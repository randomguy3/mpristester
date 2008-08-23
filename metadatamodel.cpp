/*
 * Copyright 2008  Alex Merry <alex.merry@kdemail.net>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "metadatamodel.h"

MetadataModel::MetadataModel(QObject* parent)
    : QAbstractTableModel(parent)
{
}
MetadataModel::MetadataModel(const QVariantMap& metadata, QObject* parent)
    : QAbstractTableModel(parent),
      m_metadata(metadata),
      m_keys(metadata.keys())
{
}

int MetadataModel::rowCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent)

    return m_metadata.count();
}

int MetadataModel::columnCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent)

    return 2;
}

QVariant MetadataModel::data(const QModelIndex& index, int role) const
{
    if (index.isValid() && index.column() < 2 && index.row() < rowCount()) {
        if (role == Qt::DisplayRole) {
            QString key = m_keys[index.row()];
            if (index.column() == 0) {
                return key;
            } else {
                return m_metadata[key];
            }
        }
    }
    return QVariant();
}

void MetadataModel::setMetadata(const QVariantMap& metadata)
{
    m_metadata = metadata;
    m_keys = m_metadata.keys();
    reset();
}

// vim: sw=4 sts=4 et tw=100
