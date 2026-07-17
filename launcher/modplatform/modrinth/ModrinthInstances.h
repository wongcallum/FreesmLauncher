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
 * on different backends).
 */
struct ModrinthInstance {
    QString id;
    QString name;
    QString apiUrl;
    QString metaEntryBase;
};

/**
 * All configured Modrinth instances.
 *
 * Index 0 is always the public modrinth.com backend. Additional instances are
 * parsed once from the `FREESM_MODRINTH_INSTANCES` environment variable, whose
 * value is a list of `Name|https://host/v2` entries separated by `;`, e.g.
 *
 *     FREESM_MODRINTH_INSTANCES="Home|http://nix-modrinth.ts.net:8000/v2"
 */
const QList<ModrinthInstance>& modrinthInstances();

/** The public modrinth.com instance (index 0 of modrinthInstances()). */
const ModrinthInstance& defaultModrinthInstance();
