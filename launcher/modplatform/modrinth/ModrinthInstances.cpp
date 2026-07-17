// SPDX-License-Identifier: GPL-3.0-only

#include "modplatform/modrinth/ModrinthInstances.h"

#include "BuildConfig.h"

#include <QProcessEnvironment>
#include <QRegularExpression>

static QString slugify(const QString& name)
{
    static const QRegularExpression nonAlnum("[^a-z0-9]+");
    QString slug = name.toLower().replace(nonAlnum, "-");
    while (slug.startsWith('-'))
        slug.remove(0, 1);
    while (slug.endsWith('-'))
        slug.chop(1);
    return slug;
}

static QList<ModrinthInstance> buildInstances()
{
    QList<ModrinthInstance> list;

    // The public modrinth.com backend is always present and always first, so
    // the launcher never loses access to it regardless of configuration.
    list.append({ "modrinth", "Modrinth", BuildConfig.MODRINTH_PROD_URL, "ModrinthPacks" });

    const QString env = QProcessEnvironment::systemEnvironment().value("FREESM_MODRINTH_INSTANCES");
    const auto entries = env.split(';', Qt::SkipEmptyParts);
    for (const auto& entry : entries) {
        const int sep = entry.indexOf('|');
        if (sep <= 0)
            continue;

        QString name = entry.left(sep).trimmed();
        QString url = entry.mid(sep + 1).trimmed();
        while (url.endsWith('/'))
            url.chop(1);
        if (name.isEmpty() || url.isEmpty())
            continue;

        QString slug = slugify(name);
        if (slug.isEmpty())
            slug = QString::number(list.size());

        // Keep ids/cache namespaces distinct from the built-in public instance.
        list.append({ "modrinth-" + slug, name, url, "ModrinthPacks-" + slug });
    }

    return list;
}

const QList<ModrinthInstance>& modrinthInstances()
{
    static const QList<ModrinthInstance> instances = buildInstances();
    return instances;
}

const ModrinthInstance& defaultModrinthInstance()
{
    return modrinthInstances().first();
}
