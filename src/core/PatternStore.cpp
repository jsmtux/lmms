/*
 * PatternStore.cpp - model-component of Pattern Editor
 *
 * Copyright (c) 2004-2014 Tobias Doerffel <tobydox/at/users.sourceforge.net>
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

#include <iostream>
#include "PatternStore.h"

#include "Clip.h"
#include "Engine.h"
#include "PatternTrack.h"
#include "Song.h"

namespace lmms
{


PatternStore::PatternStore() :
	m_trackContainer(this),
	m_patternComboBoxModel(this)
{
	connect(&m_patternComboBoxModel, &ComboBoxModel::dataChanged,
			Engine::getSong(), &Song::updatePatternTracks);
	// we *always* want to receive updates even in case pattern actually did
	// not change upon setCurrentPattern()-call
	connect(&m_patternComboBoxModel, &ComboBoxModel::dataUnchanged,
			Engine::getSong(), &Song::updatePatternTracks);
}




bool PatternStore::play(TimePos start, fpp_t frames, f_cnt_t offset, int clipNum)
{
	bool notePlayed = false;

	if (lengthOfPattern(clipNum) <= 0)
	{
		return false;
	}

	start = start % (lengthOfPattern(clipNum) * TimePos::ticksPerBar());

	for (Track * t : m_trackContainer.tracks())
	{
		if (t->play(start, frames, offset, clipNum))
		{
			notePlayed = true;
		}
	}

	return notePlayed;
}



bar_t PatternStore::lengthOfPattern(int pattern) const
{
	TimePos maxLength = TimePos::ticksPerBar();

	for (Track * t : m_trackContainer.tracks())
	{
		// Don't create Clips here if they don't exist
		if (pattern < t->numOfClips())
		{
			maxLength = qMax(maxLength, t->getClip(pattern)->length());
		}
	}

	return maxLength.nextFullBar();
}




void PatternStore::removePattern(int pattern)
{
	for (Track * t : m_trackContainer.tracks())
	{
		delete t->getClip(pattern);
		t->removeBar(pattern * DefaultTicksPerBar);
	}
	if (pattern <= currentPattern())
	{
		setCurrentPattern(qMax(currentPattern() - 1, 0));
	}
}




void PatternStore::swapPattern(int pattern1, int pattern2)
{
	for (Track * t : m_trackContainer.tracks())
	{
		t->swapPositionOfClips(pattern1, pattern2);
	}
	updateComboBox();
}




void PatternStore::updatePatternTrack(Clip* clip)
{
	PatternTrack * t = PatternTrack::findPatternTrack(clip->startPosition() / DefaultTicksPerBar);
	if (t != nullptr)
	{
		t->dataChanged();
	}
}




void PatternStore::fixIncorrectPositions()
{
	for (Track * t : m_trackContainer.tracks())
	{
		for (int i = 0; i < Engine::getSong()->numOfPatterns(); ++i)
		{
			t->getClip(i)->movePosition(TimePos(i, 0));
		}
	}
}




void PatternStore::play()
{
	if (Engine::getSong()->playMode() != Song::Mode_PlayPattern)
	{
		Engine::getSong()->playPattern();
	}
	else
	{
		Engine::getSong()->togglePause();
	}
}




void PatternStore::stop()
{
	Engine::getSong()->stop();
}




void PatternStore::updateComboBox()
{
	std::cout << "Getting current pattern\n";
	const int curPattern = currentPattern();
	std::cout << "Current pattern is " << curPattern << std::endl;

	m_patternComboBoxModel.clear();
	std::cout << "clearing combobox model" << std::endl;

	for (int i = 0; i < Engine::getSong()->numOfPatterns(); ++i)
	{
		std::cout << "Getting pattern " << i << std::endl;
		PatternTrack* pt = PatternTrack::findPatternTrack(i);
		std::cout << "Pointer is " << pt << std::endl;
		m_patternComboBoxModel.addItem(pt->name());
		std::cout << "Adding item" << std::endl;
	}
	std::cout << "Setting pattern" << std::endl;
	setCurrentPattern(curPattern);
	std::cout << "Pattern set!" << std::endl;
}

void PatternStore::createClipsForPattern(int pattern)
{
	for (Track * t : m_trackContainer.tracks())
	{
		t->createClipsForPattern(pattern);
	}
}

AutomatedValueMap PatternStore::automatedValuesAt(TimePos time, int clipNum) const
{
	Q_ASSERT(clipNum >= 0);
	Q_ASSERT(time.getTicks() >= 0);

	auto lengthBars = lengthOfPattern(clipNum);
	auto lengthTicks = lengthBars * TimePos::ticksPerBar();
	if (time > lengthTicks)
	{
		time = lengthTicks;
	}

	return m_trackContainer.automatedValuesFromAllTracks(time + (TimePos::ticksPerBar() * clipNum), clipNum);
}


} // namespace lmms