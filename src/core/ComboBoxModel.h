/*
 * ComboBoxModel.h - declaration of class ComboBoxModel
 *
 * Copyright (c) 2008-2014 Tobias Doerffel <tobydox/at/users.sourceforge.net>
 *
 * This file is part of LMMS - https://lmms.io
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this program (see COPYING); if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301 USA.
 *
 */

#ifndef LMMS_COMBOBOX_MODEL_H
#define LMMS_COMBOBOX_MODEL_H

#include <memory>
#include <utility>
#include <vector>

#include "AutomatableModel.h"
#include "embed.h"

namespace lmms
{

class LMMS_EXPORT ComboBoxModelWrapper : public IComboBoxModelWrapper
{
public:
	ComboBoxModelWrapper( QObject* parent = nullptr,
					const QString& displayName = QString()) :
		m_model( 0, 0, 0, parent, displayName )
	{
	}

	~ComboBoxModelWrapper()
	{
		clear();
	}

	void addItem( QString item, std::unique_ptr<PixmapLoader> loader = nullptr ) override {
		m_items.emplace_back( std::move(item), std::move(loader) );
		m_model.setRange( 0, m_items.size() - 1 );
	}

	void replaceItem(std::size_t index, QString item, std::unique_ptr<PixmapLoader> loader = nullptr) override {
		assert(index < m_items.size());
		m_items[index] = Item(std::move(item), std::move(loader));
		emit m_model.propertiesChanged();
	}

	void clear() override {
		m_model.setRange( 0, 0 );

		m_items.clear();

		emit m_model.propertiesChanged();
	}

	int findText( const QString& txt ) const override {
		for( auto it = m_items.begin(); it != m_items.end(); ++it )
		{
			if( ( *it ).first == txt )
			{
				return it - m_items.begin();
			}
		}
		return -1;
	}

	QString currentText() const override
	{
		return ( size() > 0 && m_model.value() < size() ) ? m_items[m_model.value()].first : QString();
	}

	const PixmapLoader* currentData() const override
	{
		return m_items[m_model.value()].second.get();
	}

	const QString & itemText( int i ) const override
	{
		return m_items[qBound<int>( m_model.minValue(), i,  m_model.maxValue() )].first;
	}

	const PixmapLoader* itemPixmap( int i ) const override
	{
		return m_items[qBound<int>( m_model.minValue(), i, m_model.maxValue() )].second.get();
	}

	int value() const {
		return m_model.value();
	}

	void setValue(int value) override{
		return m_model.setValue(value);
	}

	int size() const override
	{
		return m_items.size();
	}

	IntModel* model() {
		return &m_model;
	}


	const IntModel* model() const {
		return &m_model;
	}

	IIntAutomatableModel* wrappedModel() override {
		return &m_model;
	}


private:
	using Item = std::pair<QString, std::unique_ptr<PixmapLoader>>;

	IntModel m_model;

	std::vector<Item> m_items;

} ;

} // namespace lmms

#endif // LMMS_COMBOBOX_MODEL_H
