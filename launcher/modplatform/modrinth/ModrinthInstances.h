// SPDX-License-Identifier: GPL-3.0-only
//
// Support for browsing multiple Modrinth-API instances (e.g. self-hosted
// labrinth deployments) side by side, each as its own provider tab.

#pragma once

#include <QList>
#include <QString>

/**
 * A single Modrinth-API backend the launcher can browse.
 *
 * `id` / `metaEntryBase` must be unique per instance: the former keys the
 * provider tab, the latter namespaces the on-disk metadata/icon cache. Two
 * instances sharing either would collide (a project id means different things
 * on different backends). Both are derived from the display name.
 */
struct ModrinthInstance {
    QString id;
    QString name;
    QString apiUrl;
    QString metaEntryBase;
};

/**
 * A user-configured extra backend, as edited in the settings page.
 *
 * Only the human-facing fields are stored; the stable `id`/`metaEntryBase` are
 * derived from the name when these are materialised into ModrinthInstance
 * entries (see modrinthInstances()).
 */
struct ModrinthUserInstance {
    QString name;
    QString apiUrl;
};

/**
 * All configured Modrinth instances.
 *
 * Index 0 is always the public modrinth.com backend, so the launcher never
 * loses access to it regardless of configuration. The rest come from the user's
 * configured extra instances (see loadUserModrinthInstances()). Rebuilt from
 * settings on every call so edits made in the settings page take effect the next
 * time a download dialog is opened, without a restart.
 */
QList<ModrinthInstance> modrinthInstances();

/** The public modrinth.com instance (index 0 of modrinthInstances()). */
ModrinthInstance defaultModrinthInstance();

/**
 * The user-configured extra instances, loaded from / saved to application
 * settings. The settings page reads and writes these; modrinthInstances()
 * layers them on top of the built-in public backend.
 */
QList<ModrinthUserInstance> loadUserModrinthInstances();
void saveUserModrinthInstances(const QList<ModrinthUserInstance>& instances);
