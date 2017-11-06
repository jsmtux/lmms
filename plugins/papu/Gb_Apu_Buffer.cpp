/*
 * Gb_Apu_Buffer.cpp - Gb_Apu subclass which allows direct buffer access
 * Copyright (c) 2017 Tres Finocchiaro <tres.finocchiaro/at/gmail.com>
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
#include "Gb_Apu.h"
#include "Gb_Apu_Buffer.h"

blip_time_t const FRAME_LENGTH = 70224;
long const CLOCK_RATE = 4194304;

Gb_Apu_Buffer::Gb_Apu_Buffer() : m_time(0) {}
Gb_Apu_Buffer::~Gb_Apu_Buffer() {}

void Gb_Apu_Buffer::write_register(blip_time_t ignore, unsigned addr, int data) {
	Gb_Apu::write_register(clock(), addr, data);
}

int Gb_Apu_Buffer::read_register(blip_time_t ignore, unsigned addr) {
	return Gb_Apu::read_register(clock(), addr);
}

void Gb_Apu_Buffer::end_frame(blip_time_t ignore) {
	m_time = 0;
	Gb_Apu::end_frame(FRAME_LENGTH);
	m_buf.end_frame(FRAME_LENGTH);
}

// Sets specified sample rate and hard-coded clock rate in Multi_Buffer
blargg_err_t Gb_Apu_Buffer::set_sample_rate(long rate) {
	Gb_Apu_Buffer::output(m_buf.center(), m_buf.left(), m_buf.right());
	m_buf.clock_rate(CLOCK_RATE);
	return m_buf.set_sample_rate(rate);
}

// Wrap Multi_Buffer::samples_avail()
long Gb_Apu_Buffer::samples_avail() const {
	return m_buf.samples_avail();
}

// Wrap Multi_Buffer::read_samples(...)
long Gb_Apu_Buffer::read_samples(sample_t* out, long count) {
	return m_buf.read_samples(out, count);
}

void Gb_Apu_Buffer::bass_freq(int freq) {
	m_buf.bass_freq(freq);
}

