// SPDX-License-Identifier: GPL-3.0-only

#include "modplatform/modrinth/ModrinthInstances.h"

#include "Application.h"
#include "BuildConfig.h"
#include "settings/SettingsObject.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QRegularExpression>
#include <QSet>

namespace {
// Settings key holding the extra instances as a JSON array of { name, url }.
const char* const kSettingKey = "ExtraModrinthInstances";

QString slugify(const QString& name)
{
    static const QRegularExpression nonAlnum("[^a-z0-9]+");
    QString slug = name.toLower().replace(nonAlnum, "-");
    while (slug.startsWith('-'))
        slug.remove(0, 1);
    while (slug.endsWith('-'))
        slug.chop(1);
    return slug;
}

// Trim whitespace and drop any trailing slashes so two URLs that differ only by
// a trailing '/' don't produce a doubled separator when we append paths.
QString normalizeUrl(QString url)
{
    url = url.trimmed();
    while (url.endsWith('/'))
        url.chop(1);
    return url;
}
}  // namespace

QList<ModrinthUserInstance> loadUserModrinthInstances()
{
    QList<ModrinthUserInstance> list;

    const QString raw = APPLICATION->settings()->get(kSettingKey).toString();
    const QJsonDocument doc = QJsonDocument::fromJson(raw.toUtf8());
    if (!doc.isArray())
        return list;

    for (const QJsonValue& value : doc.array()) {
        const QJsonObject obj = value.toObject();
        ModrinthUserInstance inst{ obj.value("name").toString().trimmed(), normalizeUrl(obj.value("url").toString()) };
        if (inst.name.isEmpty() || inst.apiUrl.isEmpty())
            continue;
        list.append(inst);
    }

    return list;
}

void saveUserModrinthInstances(const QList<ModrinthUserInstance>& instances)
{
    QJsonArray arr;
    for (const auto& inst : instances) {
        const QString name = inst.name.trimmed();
        const QString url = normalizeUrl(inst.apiUrl);
        if (name.isEmpty() || url.isEmpty())
            continue;

        QJsonObject obj;
        obj["name"] = name;
        obj["url"] = url;
        arr.append(obj);
    }

    APPLICATION->settings()->set(kSettingKey, QString::fromUtf8(QJsonDocument(arr).toJson(QJsonDocument::Compact)));
}

QList<ModrinthInstance> modrinthInstances()
{
    QList<ModrinthInstance> list;

    // The public modrinth.com backend is always present and always first.
    const ModrinthInstance def = defaultModrinthInstance();
    list.append(def);

    // Keep ids/cache namespaces distinct from each other and the public instance:
    // a colliding slug would make two tabs share one on-disk metadata cache.
    QSet<QString> usedSlugs{ slugify(def.name) };
    for (const auto& user : loadUserModrinthInstances()) {
        QString slug = slugify(user.name);
        if (slug.isEmpty())
            slug = QString::number(list.size());

        const QString baseSlug = slug;
        for (int suffix = 2; usedSlugs.contains(slug); ++suffix)
            slug = baseSlug + "-" + QString::number(suffix);
        usedSlugs.insert(slug);

        list.append({ "modrinth-" + slug, user.name, user.apiUrl, "ModrinthPacks-" + slug });
    }

    return list;
}

ModrinthInstance defaultModrinthInstance()
{
    return { "modrinth", "Modrinth", BuildConfig.MODRINTH_PROD_URL, "ModrinthPacks" };
}
