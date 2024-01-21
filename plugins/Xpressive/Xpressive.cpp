/*
 * Xpressive.cpp - Instrument which uses a mathematical formula parser
 *
 * Copyright (c) 2016-2017 Orr Dvori
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

#include "Xpressive.h"

#include <QDomElement>
#include <QPlainTextEdit>

#include "AudioEngine.h"
#include "Engine.h"
#include "GuiApplication.h"
#include "InstrumentTrack.h"
#include "MainWindow.h"
#include "NotePlayHandle.h"
#include "Song.h"

#include "widgets/Graph.h"
#include "widgets/Knob.h"
#include "widgets/LedCheckBox.h"
#include "widgets/PixmapButton.h"

#include "base64.h"
#include "lmms_constants.h"

#include "embed.h"

#include "ExprSynth.h"

#include "plugin_export.h"

namespace lmms
{


extern "C" {

Plugin::Descriptor PLUGIN_EXPORT xpressive_plugin_descriptor = { LMMS_STRINGIFY(
	PLUGIN_NAME), "Xpressive", QT_TRANSLATE_NOOP("PluginBrowser",
	"Mathematical expression parser"), "Orr Dvori", 0x0100,
	Plugin::Instrument, new PluginPixmapLoader("logo"), nullptr, nullptr };

}



/*
 * nice test:
O1 -> trianglew(2t*f)*(0.5+0.5sinew(12*A1*t+0.5))+sinew(t*f)*(0.5+0.5sinew(12*A1*t))
O2 -> trianglew(2t*f)*(0.5+0.5sinew(12*A1*t))+sinew(t*f)*(0.5+0.5sinew(12*A1*t+0.5))
*/


/***********************************************************************
 *
 *	class Xpressive
 *
 *	lmms - plugin
 *
 ***********************************************************************/
#define GRAPH_LENGTH 4096

Xpressive::Xpressive(InstrumentTrack* instrument_track) :
	QWidgetInstrumentPlugin(instrument_track, &xpressive_plugin_descriptor),
	m_graphO1(-1.0f, 1.0f, 360, model()),
	m_graphO2(-1.0f, 1.0f, 360, model()),
	m_graphW1(-1.0f, 1.0f, GRAPH_LENGTH, model()),
	m_graphW2(-1.0f, 1.0f, GRAPH_LENGTH, model()),
	m_graphW3(-1.0f, 1.0f, GRAPH_LENGTH, model()),
	m_rawgraphW1(-1.0f, 1.0f, GRAPH_LENGTH, model()),
	m_rawgraphW2(-1.0f, 1.0f, GRAPH_LENGTH, model()),
	m_rawgraphW3(-1.0f, 1.0f, GRAPH_LENGTH, model()),
	m_selectedGraph(0, 0, 6, model(), tr("Selected graph")),
	m_parameterA1(1, -1.0f, 1.0f, 0.01f, model(), tr("A1")),
	m_parameterA2(1, -1.0f, 1.0f, 0.01f, model(), tr("A2")),
	m_parameterA3(1, -1.0f, 1.0f, 0.01f, model(), tr("A3")),
	m_smoothW1(0, 0.0f, 70.0f, 1.0f, model(), tr("W1 smoothing")),
	m_smoothW2(0, 0.0f, 70.0f, 1.0f, model(), tr("W2 smoothing")),
	m_smoothW3(0, 0.0f, 70.0f, 1.0f, model(), tr("W3 smoothing")),
	m_interpolateW1(false, model()),
	m_interpolateW2(false, model()),
	m_interpolateW3(false, model()),
	m_panning1( 1, -1.0f, 1.0f, 0.01f, model(), tr("Panning 1")),
	m_panning2(-1, -1.0f, 1.0f, 0.01f, model(), tr("Panning 2")),
	m_relTransition(50.0f, 0.0f, 500.0f, 1.0f, model(), tr("Rel trans")),
	m_W1(GRAPH_LENGTH),
	m_W2(GRAPH_LENGTH),
	m_W3(GRAPH_LENGTH),
	m_exprValid(false, model())
{
	m_outputExpression[0]="sinew(integrate(f*(1+0.05sinew(12t))))*(2^(-(1.1+A2)*t)*(0.4+0.1(1+A3)+0.4sinew((2.5+2A1)t))^2)";
	m_outputExpression[1]="expw(integrate(f*atan(500t)*2/pi))*0.5+0.12";
}

void Xpressive::saveSettings(QDomDocument & _doc, QDomElement & _this) {

	// Save plugin version
	_this.setAttribute("version", "0.1");
	_this.setAttribute("O1", QString(m_outputExpression[0]));
	_this.setAttribute("O2", QString(m_outputExpression[1]));
	_this.setAttribute("W1", QString(m_wavesExpression[0]));
	// Save sample shape base64-encoded
	QString sampleString;
	base64::encode( (const char*)m_rawgraphW1.samples(),
		 m_rawgraphW1.length() * sizeof(float), sampleString );
	_this.setAttribute( "W1sample", sampleString );

	_this.setAttribute("W2", QString(m_wavesExpression[1]));
	base64::encode( (const char*)m_rawgraphW2.samples(),
		 m_rawgraphW2.length() * sizeof(float), sampleString );
	_this.setAttribute( "W2sample", sampleString );
	_this.setAttribute("W3", QString(m_wavesExpression[2]));
	base64::encode( (const char*)m_rawgraphW3.samples(),
		 m_rawgraphW3.length() * sizeof(float), sampleString );
	_this.setAttribute( "W3sample", sampleString );
	m_smoothW1.saveSettings(_doc,_this,"smoothW1");
	m_smoothW2.saveSettings(_doc,_this,"smoothW2");
	m_smoothW3.saveSettings(_doc,_this,"smoothW3");
	m_interpolateW1.saveSettings(_doc,_this,"interpolateW1");
	m_interpolateW2.saveSettings(_doc,_this,"interpolateW2");
	m_interpolateW3.saveSettings(_doc,_this,"interpolateW3");
	m_parameterA1.saveSettings(_doc,_this,"A1");
	m_parameterA2.saveSettings(_doc,_this,"A2");
	m_parameterA3.saveSettings(_doc,_this,"A3");
	m_panning1.saveSettings(_doc,_this,"PAN1");
	m_panning2.saveSettings(_doc,_this,"PAN2");
	m_relTransition.saveSettings(_doc,_this,"RELTRANS");

}

void Xpressive::loadSettings(const QDomElement & _this) {

	m_outputExpression[0]=_this.attribute( "O1").toLatin1();
	m_outputExpression[1]=_this.attribute( "O2").toLatin1();
	m_wavesExpression[0]=_this.attribute( "W1").toLatin1();
	m_wavesExpression[1]=_this.attribute( "W2").toLatin1();
	m_wavesExpression[2]=_this.attribute( "W3").toLatin1();

	m_smoothW1.loadSettings(_this,"smoothW1");
	m_smoothW2.loadSettings(_this,"smoothW2");
	m_smoothW3.loadSettings(_this,"smoothW3");
	m_interpolateW1.loadSettings(_this,"interpolateW1");
	m_interpolateW2.loadSettings(_this,"interpolateW2");
	m_interpolateW3.loadSettings(_this,"interpolateW3");
	m_parameterA1.loadSettings(_this,"A1");
	m_parameterA2.loadSettings(_this,"A2");
	m_parameterA3.loadSettings(_this,"A3");
	m_panning1.loadSettings(_this,"PAN1");
	m_panning2.loadSettings(_this,"PAN2");
	m_relTransition.loadSettings(_this,"RELTRANS");

	int size = 0;
	char * dst = 0;
	base64::decode( _this.attribute( "W1sample"), &dst, &size );

	m_rawgraphW1.setSamples( (float*) dst );
	delete[] dst;
	base64::decode( _this.attribute( "W2sample"), &dst, &size );

	m_rawgraphW2.setSamples( (float*) dst );
	delete[] dst;
	base64::decode( _this.attribute( "W3sample"), &dst, &size );

	m_rawgraphW3.setSamples( (float*) dst );
	delete[] dst;

	smooth(m_smoothW1.value(),&m_rawgraphW1,&m_graphW1);
	smooth(m_smoothW2.value(),&m_rawgraphW2,&m_graphW2);
	smooth(m_smoothW3.value(),&m_rawgraphW3,&m_graphW3);
	m_W1.copyFrom(&m_graphW1);
	m_W2.copyFrom(&m_graphW2);
	m_W3.copyFrom(&m_graphW3);
}


QString Xpressive::nodeName() const {
	return (xpressive_plugin_descriptor.name);
}

void Xpressive::playNote(NotePlayHandle* nph, sampleFrame* working_buffer) {
	m_A1=m_parameterA1.value();
	m_A2=m_parameterA2.value();
	m_A3=m_parameterA3.value();

	if (nph->totalFramesPlayed() == 0 || nph->m_pluginData == nullptr) {

		auto exprO1 = new ExprFront(m_outputExpression[0].constData(),
			Engine::audioEngine()->processingSampleRate()); // give the "last" function a whole second
		auto exprO2 = new ExprFront(m_outputExpression[1].constData(), Engine::audioEngine()->processingSampleRate());

		auto init_expression_step1 = [this, nph](ExprFront* e) { //lambda function to init exprO1 and exprO2
			//add the constants and the variables to the expression.
			e->add_constant("key", nph->key());//the key that was pressed.
			e->add_constant("bnote", nph->instrumentTrack()->baseNote()); // the base note
			e->add_constant("srate", Engine::audioEngine()->processingSampleRate());// sample rate of the audio engine
			e->add_constant("v", nph->getVolume() / 255.0); //volume of the note.
			e->add_constant("tempo", Engine::getSong()->getTempo());//tempo of the song.
			e->add_variable("A1", m_A1);//A1,A2,A3: general purpose input controls.
			e->add_variable("A2", m_A2);
			e->add_variable("A3", m_A3);
		};
		init_expression_step1(exprO1);
		init_expression_step1(exprO2);

		m_W1.setInterpolate(m_interpolateW1.value());//set interpolation according to the user selection.
		m_W2.setInterpolate(m_interpolateW2.value());
		m_W3.setInterpolate(m_interpolateW3.value());
		nph->m_pluginData = new ExprSynth(&m_W1, &m_W2, &m_W3, exprO1, exprO2, nph,
				Engine::audioEngine()->processingSampleRate(), &m_panning1, &m_panning2, m_relTransition.value());
	}

	auto ps = static_cast<ExprSynth*>(nph->m_pluginData);
	const fpp_t frames = nph->framesLeftForCurrentPeriod();
	const f_cnt_t offset = nph->noteOffset();

	ps->renderOutput(frames, working_buffer + offset);

	instrumentTrack()->processAudioBuffer(working_buffer, frames + offset, nph);
}

void Xpressive::deleteNotePluginData(NotePlayHandle* nph) {
	delete static_cast<ExprSynth *>(nph->m_pluginData);
}

gui::InstrumentView* Xpressive::instantiateView(QWidget* parent) {
	return (new gui::XpressiveView(this, parent));
}


void Xpressive::smooth(float smoothness,const gui::graphModel * in,gui::graphModel * out)
{
	out->setSamples(in->samples());
	if (smoothness>0)
	{
		const int guass_size = (int)(smoothness * 5) | 1;
		const int guass_center = guass_size/2;
		const float delta = smoothness;
		const float a= 1.0f / (sqrtf(2.0f * F_PI) * delta);
		auto const guassian = new float[guass_size];
		float sum = 0.0f;
		float temp = 0.0f;
		int i;
		for (i = 0; i < guass_size; i++ )
		{
			temp = (i - guass_center) / delta;
			sum += guassian[i] = a * powf(F_E, -0.5f * temp * temp);
		}
		for (i = 0; i < guass_size; i++ )
		{
			guassian[i] = guassian[i] / sum;
		}
		out->convolve(guassian, guass_size, guass_center);
		delete [] guassian;
	}
}


namespace gui
{


class XpressiveKnob: public Knob {
public:
	void setStyle()
	{
		setFixedSize(29, 29);
		setCenterPointX(14.5);
		setCenterPointY(14.5);
		setInnerRadius(4);
		setOuterRadius(9);
		setTotalAngle(300.0);
		setLineWidth(3);
	}
	XpressiveKnob(FloatModel* _model, QWidget * _parent, const QString & _name) :
		Knob(knobStyled, _model, _parent,_name) {
		setStyle();
	}
	XpressiveKnob(FloatModel* _model, QWidget * _parent) :
		Knob(knobStyled, _model, _parent) {
		setStyle();
	}

};


XpressiveView::XpressiveView(Xpressive * _instrument, QWidget * _parent) :
	InstrumentViewImpl(_instrument, _parent, true)

{
	const int COL_KNOBS = 191;
	const int BASE_START = 2;
	const int ROW_KNOBSA1 = BASE_START;
	const int ROW_KNOBSA2 = BASE_START + 32;
	const int ROW_KNOBSA3 = BASE_START + 64;
	const int ROW_KNOBSP1 = BASE_START + 100;
	const int ROW_KNOBSP2 = BASE_START + 100 + 32;
	const int ROW_KNOBREL = BASE_START + 100 + 64;
	const int ROW_BTN = BASE_START + 85;
	const int ROW_WAVEBTN = BASE_START + 233 - 26;
	const int EXPR_TEXT_Y = BASE_START + 102;
	const int EXPR_TEXT_H = 90;

	setAutoFillBackground(true);

	const auto& create_graph = [this](gui::graphModel* _model){
		QPalette pal;

		pal.setBrush(backgroundRole(), PLUGIN_NAME::getIconPixmap("artwork"));
		setPalette(pal);
		Graph* graph = new Graph(_model, this, Graph::LinearStyle, 180, 81);
		graph->move(3, BASE_START + 1);
		graph->setAutoFillBackground(true);
		graph->setGraphColor(QColor(255, 255, 255));
		graph->setEnabled(false);

		graph->setToolTip(tr("Draw your own waveform here "
				"by dragging your mouse on this graph."));

		pal = QPalette();
		pal.setBrush(backgroundRole(), PLUGIN_NAME::getIconPixmap("wavegraph"));
		graph->setPalette(pal);
		connect(graph, SIGNAL(drawn()), this,
				SLOT(graphDrawn()));
		return graph;
	};

	m_graphO1 = create_graph(&m_instrument->graphO1());
	m_graphO2 = create_graph(&m_instrument->graphO2());
	m_graphW1 = create_graph(&m_instrument->graphW1());
	m_graphW2 = create_graph(&m_instrument->graphW2());
	m_graphW3 = create_graph(&m_instrument->graphW3());

	PixmapButton * m_w1Btn;
	PixmapButton * m_w2Btn;
	PixmapButton * m_w3Btn;
	PixmapButton * m_o1Btn;
	PixmapButton * m_o2Btn;
	PixmapButton * m_helpBtn;

	m_w1Btn = new PixmapButton(this, nullptr);
	m_w1Btn->move(3, ROW_BTN);
	m_w1Btn->setActiveGraphic(PLUGIN_NAME::getIconPixmap("w1_active"));
	m_w1Btn->setInactiveGraphic(PLUGIN_NAME::getIconPixmap("w1_inactive"));
	m_w1Btn->setToolTip(tr("Select oscillator W1"));

	m_w2Btn = new PixmapButton(this, nullptr);
	m_w2Btn->move(26, ROW_BTN);
	m_w2Btn->setActiveGraphic(PLUGIN_NAME::getIconPixmap("w2_active"));
	m_w2Btn->setInactiveGraphic(PLUGIN_NAME::getIconPixmap("w2_inactive"));
	m_w2Btn->setToolTip(tr("Select oscillator W2"));

	m_w3Btn = new PixmapButton(this, nullptr);
	m_w3Btn->move(49, ROW_BTN);
	m_w3Btn->setActiveGraphic(PLUGIN_NAME::getIconPixmap("w3_active"));
	m_w3Btn->setInactiveGraphic(PLUGIN_NAME::getIconPixmap("w3_inactive"));
	m_w3Btn->setToolTip(tr("Select oscillator W3"));

	m_o1Btn = new PixmapButton(this, nullptr);
	m_o1Btn->move(79, ROW_BTN);
	m_o1Btn->setActiveGraphic(PLUGIN_NAME::getIconPixmap("o1_active"));
	m_o1Btn->setInactiveGraphic(PLUGIN_NAME::getIconPixmap("o1_inactive"));
	m_o1Btn->setToolTip(tr("Select output O1"));

	m_o2Btn = new PixmapButton(this, nullptr);
	m_o2Btn->move(101, ROW_BTN);
	m_o2Btn->setActiveGraphic(PLUGIN_NAME::getIconPixmap("o2_active"));
	m_o2Btn->setInactiveGraphic(PLUGIN_NAME::getIconPixmap("o2_inactive"));
	m_o2Btn->setToolTip(tr("Select output O2"));

	m_helpBtn = new PixmapButton(this, nullptr);
	m_helpBtn->move(133, ROW_BTN);
	m_helpBtn->setActiveGraphic(PLUGIN_NAME::getIconPixmap("help_active"));
	m_helpBtn->setInactiveGraphic(PLUGIN_NAME::getIconPixmap("help_inactive"));
	m_helpBtn->setToolTip(tr("Open help window"));

	m_selectedGraphGroup = new automatableButtonGroup(&m_instrument->selectedGraph(), this);
	m_selectedGraphGroup->addButton(m_w1Btn);
	m_selectedGraphGroup->addButton(m_w2Btn);
	m_selectedGraphGroup->addButton(m_w3Btn);
	m_selectedGraphGroup->addButton(m_o1Btn);
	m_selectedGraphGroup->addButton(m_o2Btn);

	m_sinWaveBtn = new PixmapButton(this, new BoolModel(false, this), tr("Sine wave"));
	m_sinWaveBtn->move(4, ROW_WAVEBTN);
	m_sinWaveBtn->setActiveGraphic(embed::getIconPixmap("sin_wave_active"));
	m_sinWaveBtn->setInactiveGraphic(embed::getIconPixmap("sin_wave_inactive"));
	m_sinWaveBtn->setToolTip(tr("Sine wave"));

	m_moogWaveBtn = new PixmapButton(this, new BoolModel(false, this), tr("Moog-saw wave"));
	m_moogWaveBtn->move(4, ROW_WAVEBTN-14);
	m_moogWaveBtn->setActiveGraphic(
		embed::getIconPixmap( "moog_saw_wave_active" ) );
	m_moogWaveBtn->setInactiveGraphic(embed::getIconPixmap("moog_saw_wave_inactive"));
	m_moogWaveBtn->setToolTip(tr("Moog-saw wave"));

	m_expWaveBtn = new PixmapButton(this, new BoolModel(false, this), tr("Exponential wave"));
	m_expWaveBtn->move(4 +14, ROW_WAVEBTN-14);
	m_expWaveBtn->setActiveGraphic(embed::getIconPixmap( "exp_wave_active" ) );
	m_expWaveBtn->setInactiveGraphic(embed::getIconPixmap( "exp_wave_inactive" ) );
	m_expWaveBtn->setToolTip(tr("Exponential wave"));

	m_sawWaveBtn = new PixmapButton(this, new BoolModel(false, this), tr("Saw wave"));
	m_sawWaveBtn->move(4 + 14 * 2, ROW_WAVEBTN-14);
	m_sawWaveBtn->setActiveGraphic(embed::getIconPixmap("saw_wave_active"));
	m_sawWaveBtn->setInactiveGraphic(embed::getIconPixmap("saw_wave_inactive"));
	m_sawWaveBtn->setToolTip(tr("Saw wave"));

	m_usrWaveBtn = new PixmapButton(this, new BoolModel(false, this), tr("User-defined wave"));
	m_usrWaveBtn->move(4 + 14 * 3, ROW_WAVEBTN-14);
	m_usrWaveBtn->setActiveGraphic(embed::getIconPixmap("usr_wave_active"));
	m_usrWaveBtn->setInactiveGraphic(embed::getIconPixmap("usr_wave_inactive"));
	m_usrWaveBtn->setToolTip(tr("User-defined wave"));

	m_triangleWaveBtn = new PixmapButton(this, new BoolModel(false, this), tr("Triangle wave"));
	m_triangleWaveBtn->move(4 + 14, ROW_WAVEBTN);
	m_triangleWaveBtn->setActiveGraphic(
		embed::getIconPixmap("triangle_wave_active"));
	m_triangleWaveBtn->setInactiveGraphic(
			embed::getIconPixmap("triangle_wave_inactive"));
	m_triangleWaveBtn->setToolTip(tr("Triangle wave"));

	m_sqrWaveBtn = new PixmapButton(this, new BoolModel(false, this), tr("Square wave"));
	m_sqrWaveBtn->move(4 + 14 * 2, ROW_WAVEBTN);
	m_sqrWaveBtn->setActiveGraphic(embed::getIconPixmap("square_wave_active"));
	m_sqrWaveBtn->setInactiveGraphic(
			embed::getIconPixmap("square_wave_inactive"));
	m_sqrWaveBtn->setToolTip(tr("Square wave"));

	m_whiteNoiseWaveBtn = new PixmapButton(this, new BoolModel(false, this), tr("White noise"));
	m_whiteNoiseWaveBtn->move(4 + 14 * 3, ROW_WAVEBTN);
	m_whiteNoiseWaveBtn->setActiveGraphic(
		embed::getIconPixmap("white_noise_wave_active"));
	m_whiteNoiseWaveBtn->setInactiveGraphic(
			embed::getIconPixmap("white_noise_wave_inactive"));
	m_whiteNoiseWaveBtn->setToolTip(tr("White noise"));


	m_waveInterpolate1  = new LedCheckBox("Interpolate", &m_instrument->interpolateW1(),
		this, tr("WaveInterpolate"), LedCheckBox::Green);
	m_waveInterpolate1->move(2, 230);
	m_waveInterpolate2  = new LedCheckBox("Interpolate", &m_instrument->interpolateW2(),
		this, tr("WaveInterpolate"), LedCheckBox::Green);
	m_waveInterpolate2->move(2, 230);
	m_waveInterpolate3  = new LedCheckBox("Interpolate", &m_instrument->interpolateW3(),
		this, tr("WaveInterpolate"), LedCheckBox::Green);
	m_waveInterpolate3->move(2, 230);

	m_expressionValidToggle = new LedCheckBox("", &m_instrument->exprValid(), this, tr("ExpressionValid"),
											  LedCheckBox::Red);
	m_expressionValidToggle->move(168, EXPR_TEXT_Y+EXPR_TEXT_H-2);
	m_expressionValidToggle->setEnabled( false );


	m_expressionEditor = new QPlainTextEdit(this);
	m_expressionEditor->move(3, EXPR_TEXT_Y);
	m_expressionEditor->resize(180, EXPR_TEXT_H);

	m_generalPurposeKnob[0] = new XpressiveKnob(&m_instrument->parameterA1(), this,"A1");
	m_generalPurposeKnob[0]->setHintText(tr("General purpose 1:"), "");
	m_generalPurposeKnob[0]->move(COL_KNOBS, ROW_KNOBSA1);

	m_generalPurposeKnob[1] = new XpressiveKnob(&m_instrument->parameterA2(), this,"A2");
	m_generalPurposeKnob[1]->setHintText(tr("General purpose 2:"), "");
	m_generalPurposeKnob[1]->move(COL_KNOBS, ROW_KNOBSA2);

	m_generalPurposeKnob[2] = new XpressiveKnob(&m_instrument->parameterA3(), this,"A3");
	m_generalPurposeKnob[2]->setHintText(tr("General purpose 3:"), "");
	m_generalPurposeKnob[2]->move(COL_KNOBS, ROW_KNOBSA3);

	m_panningKnob[0] = new XpressiveKnob(&m_instrument->panning1(), this,"O1 panning");
	m_panningKnob[0]->setHintText(tr("O1 panning:"), "");
	m_panningKnob[0]->move(COL_KNOBS, ROW_KNOBSP1);

	m_panningKnob[1] = new XpressiveKnob(&m_instrument->panning2(), this,"O2 panning");
	m_panningKnob[1]->setHintText(tr("O2 panning:"), "");
	m_panningKnob[1]->move(COL_KNOBS, ROW_KNOBSP2);

	m_relKnob = new XpressiveKnob(&m_instrument->relTransition(), this,"Release transition");
	m_relKnob->setHintText(tr("Release transition:"), "ms");
	m_relKnob->move(COL_KNOBS, ROW_KNOBREL);

	const auto& createSmoothKnob = [this](FloatModel* model){
		auto smoothKnob=new Knob(knobStyled, model, this, "Smoothness");
		smoothKnob->setFixedSize(25, 25);
		smoothKnob->setCenterPointX(12.5);
		smoothKnob->setCenterPointY(12.5);
		smoothKnob->setInnerRadius(4);
		smoothKnob->setOuterRadius(9);
		smoothKnob->setTotalAngle(280.0);
		smoothKnob->setLineWidth(3);
		smoothKnob->setHintText(tr("Smoothness"), "");
		smoothKnob->move(66, EXPR_TEXT_Y + EXPR_TEXT_H + 4);
		connect(smoothKnob, SIGNAL(sliderMoved(float)), this,
				SLOT(smoothChanged()));
		return smoothKnob;
	};
	m_smoothKnob1 = createSmoothKnob(&m_instrument->smoothW1());
	m_smoothKnob2 = createSmoothKnob(&m_instrument->smoothW2());
	m_smoothKnob3 = createSmoothKnob(&m_instrument->smoothW3());


	connect(m_generalPurposeKnob[0], SIGNAL(sliderMoved(float)), this,
			SLOT(expressionChanged()));
	connect(m_generalPurposeKnob[1], SIGNAL(sliderMoved(float)), this,
			SLOT(expressionChanged()));
	connect(m_generalPurposeKnob[2], SIGNAL(sliderMoved(float)), this,
			SLOT(expressionChanged()));

	connect(m_expressionEditor, SIGNAL(textChanged()), this,
			SLOT(expressionChanged()));

	connect(m_sinWaveBtn, SIGNAL(clicked()), this, SLOT(sinWaveClicked()));
	connect(m_triangleWaveBtn, SIGNAL(clicked()), this,
			SLOT(triangleWaveClicked()));
	connect(m_expWaveBtn, SIGNAL(clicked()), this, SLOT(expWaveClicked()));
	connect(m_moogWaveBtn, SIGNAL(clicked()), this,
			SLOT(moogSawWaveClicked()));
	connect(m_sawWaveBtn, SIGNAL(clicked()), this, SLOT(sawWaveClicked()));
	connect(m_sqrWaveBtn, SIGNAL(clicked()), this, SLOT(sqrWaveClicked()));
	connect(m_whiteNoiseWaveBtn, SIGNAL(clicked()), this,
			SLOT(noiseWaveClicked()));
	connect(m_usrWaveBtn, SIGNAL(clicked()), this, SLOT(usrWaveClicked()));
	connect(m_helpBtn, SIGNAL(clicked()), this, SLOT(helpClicked()));

	connect(m_w1Btn, SIGNAL(clicked()), this, SLOT(updateLayout()));
	connect(m_w2Btn, SIGNAL(clicked()), this, SLOT(updateLayout()));
	connect(m_w3Btn, SIGNAL(clicked()), this, SLOT(updateLayout()));
	connect(m_o1Btn, SIGNAL(clicked()), this, SLOT(updateLayout()));
	connect(m_o2Btn, SIGNAL(clicked()), this, SLOT(updateLayout()));

	updateLayout();
}


void XpressiveView::expressionChanged() {
	QByteArray text = m_expressionEditor->toPlainText().toLatin1();

	switch (m_selectedGraphGroup->model()->value()) {
		case W1_EXPR:
			m_instrument->wavesExpression(0) = text;
			break;
		case W2_EXPR:
			m_instrument->wavesExpression(1) = text;
			break;
		case W3_EXPR:
			m_instrument->wavesExpression(2) = text;
			break;
		case O1_EXPR:
			m_instrument->outputExpression(0) = text;
			break;
		case O2_EXPR:
			m_instrument->outputExpression(1) = text;
			break;
	}
	if (m_wave_expr) {
		m_graphW1->setEnabled(m_smoothKnob1->model()->value() == 0 && text.size() == 0);
		m_graphW2->setEnabled(m_smoothKnob2->model()->value() == 0 && text.size() == 0);
		m_graphW3->setEnabled(m_smoothKnob3->model()->value() == 0 && text.size() == 0);
	}

	if (text.size()>0)
	{
		const unsigned int sample_rate=m_raw_graph->length();
		ExprFront expr(text.constData(),sample_rate);
		float t=0;
		const float f=10,key=5,v=0.5;
		unsigned int i;
		expr.add_variable("t", t);

		if (m_output_expr)
		{
			expr.add_constant("f", f);
			expr.add_constant("key", key);
			expr.add_constant("rel", 0);
			expr.add_constant("trel", 0);
			expr.add_constant("bnote",m_instrument->instrumentTrack()->baseNote());
			expr.add_constant("v", v);
			expr.add_constant("tempo", Engine::getSong()->getTempo());
			expr.add_constant("A1", m_instrument->parameterA1().value());
			expr.add_constant("A2", m_instrument->parameterA2().value());
			expr.add_constant("A3", m_instrument->parameterA3().value());
			expr.add_cyclic_vector("W1",m_instrument->graphW1().samples(),m_instrument->graphW1().length());
			expr.add_cyclic_vector("W2",m_instrument->graphW2().samples(),m_instrument->graphW2().length());
			expr.add_cyclic_vector("W3",m_instrument->graphW3().samples(),m_instrument->graphW3().length());
		}
		expr.setIntegrate(&i,sample_rate);
		expr.add_constant("srate",sample_rate);

		const bool parse_ok=expr.compile();

		if (parse_ok) {
			m_instrument->exprValid().setValue(0);
			const int length = m_raw_graph->length();
			auto const samples = new float[length];
			for (i = 0; i < length; i++) {
				t = i / (float) length;
				samples[i] = expr.evaluate();
				if (std::isinf(samples[i]) != 0 || std::isnan(samples[i]) != 0)
					samples[i] = 0;
			}
			m_raw_graph->setSamples(samples);
			delete[] samples;
			if (m_wave_expr)
			{
				smoothChanged();
			}
			else
			{
				Engine::getSong()->setModified();
			}
		}
		else
		{
			m_instrument->exprValid().setValue(1);
			if (m_output_expr)
				m_raw_graph->clear();
		}
	}
	else
	{
		m_instrument->exprValid().setValue(0);
		if (m_output_expr)
			m_raw_graph->clear();
	}
}

void XpressiveView::smoothChanged()
{
	float smoothness=0;
	switch (m_selectedGraphGroup->model()->value()) {
	case W1_EXPR:
		smoothness=m_instrument->smoothW1().value();
		Xpressive::smooth(smoothness,m_raw_graph,m_graphW1->model());
		break;
	case W2_EXPR:
		smoothness=m_instrument->smoothW2().value();
		Xpressive::smooth(smoothness,m_raw_graph,m_graphW2->model());
		break;
	case W3_EXPR:
		smoothness=m_instrument->smoothW3().value();
		Xpressive::smooth(smoothness,m_raw_graph,m_graphW3->model());
		break;
	}
	switch (m_selectedGraphGroup->model()->value()) {
	case W1_EXPR:
		m_instrument->W1().copyFrom(m_graphW1->model());
		m_graphW1->setEnabled(m_smoothKnob1->model()->value() == 0 && m_expressionEditor->toPlainText().size() == 0);
		break;
	case W2_EXPR:
		m_instrument->W2().copyFrom(m_graphW2->model());
		m_graphW2->setEnabled(m_smoothKnob2->model()->value() == 0 && m_expressionEditor->toPlainText().size() == 0);
		break;
	case W3_EXPR:
		m_instrument->W3().copyFrom(m_graphW3->model());
		m_graphW3->setEnabled(m_smoothKnob3->model()->value() == 0 && m_expressionEditor->toPlainText().size() == 0);
		break;
	}
	Engine::getSong()->setModified();
}

void XpressiveView::graphDrawn()
{
	switch (m_selectedGraphGroup->model()->value()) {
	case W1_EXPR:
		m_raw_graph->setSamples(m_graphW1->model()->samples());
		m_instrument->W1().copyFrom(m_graphW1->model());
		break;
	case W2_EXPR:
		m_raw_graph->setSamples(m_graphW2->model()->samples());
		m_instrument->W2().copyFrom(m_graphW2->model());
		break;
	case W3_EXPR:
		m_raw_graph->setSamples(m_graphW3->model()->samples());
		m_instrument->W3().copyFrom(m_graphW3->model());
		break;
	}
	Engine::getSong()->setModified();
}

void XpressiveView::updateLayout() {
	m_output_expr=false;
	m_wave_expr=false;

	m_smoothKnob1->hide();
	m_waveInterpolate1->hide();
	m_smoothKnob2->hide();
	m_waveInterpolate2->hide();
	m_smoothKnob3->hide();
	m_waveInterpolate3->hide();
	m_graphO1->setEnabled(false);
	m_graphO2->setEnabled(false);
	m_graphW1->setEnabled(false);
	m_usrWaveBtn->hide();

	switch (m_selectedGraphGroup->model()->value()) {
	case W1_EXPR:
		m_wave_expr=true;
		m_raw_graph=&(m_instrument->rawgraphW1());
		m_expressionEditor->setPlainText(m_instrument->wavesExpression(0));
		m_graphW1->setEnabled((m_instrument->smoothW1().value() == 0 && m_instrument->wavesExpression(0).size() == 0));
		m_smoothKnob1->show();
		m_usrWaveBtn->show();
		m_waveInterpolate1->show();
		break;
	case W2_EXPR:
		m_wave_expr=true;
		m_raw_graph=&(m_instrument->rawgraphW2());
		m_expressionEditor->setPlainText(m_instrument->wavesExpression(1));
		m_graphW2->setEnabled((m_instrument->smoothW2().value() == 0 && m_instrument->wavesExpression(1).size() == 0));
		m_smoothKnob2->show();
		m_usrWaveBtn->show();
		m_waveInterpolate2->show();
		break;
	case W3_EXPR:
		m_wave_expr=true;
		m_raw_graph=&(m_instrument->rawgraphW3());
		m_expressionEditor->setPlainText(m_instrument->wavesExpression(2));
		m_graphW3->setEnabled((m_instrument->smoothW3().value() == 0 && m_instrument->wavesExpression(2).size() == 0));
		m_smoothKnob3->show();
		m_usrWaveBtn->show();
		m_waveInterpolate3->show();
		break;
	case O1_EXPR:
		m_output_expr=true;
		m_raw_graph=&(m_instrument->graphO1());
		m_expressionEditor->setPlainText(m_instrument->outputExpression(0));
		break;
	case O2_EXPR:
		m_output_expr=true;
		m_raw_graph=&(m_instrument->graphO2());
		m_expressionEditor->setPlainText(m_instrument->outputExpression(1));
		break;
	}
}

void XpressiveView::sinWaveClicked() {
	if (m_output_expr)
		m_expressionEditor->appendPlainText("sinew(integrate(f))");
	else
		m_expressionEditor->appendPlainText("sinew(t)");
	Engine::getSong()->setModified();
}

void XpressiveView::triangleWaveClicked() {
	if (m_output_expr)
		m_expressionEditor->appendPlainText("trianglew(integrate(f))");
	else
		m_expressionEditor->appendPlainText("trianglew(t)");
	Engine::getSong()->setModified();
}

void XpressiveView::sawWaveClicked() {
	if (m_output_expr)
		m_expressionEditor->appendPlainText("saww(integrate(f))");
	else
		m_expressionEditor->appendPlainText("saww(t)");
	Engine::getSong()->setModified();
}

void XpressiveView::sqrWaveClicked() {
	if (m_output_expr)
		m_expressionEditor->appendPlainText("squarew(integrate(f))");
	else
		m_expressionEditor->appendPlainText("squarew(t)");
	Engine::getSong()->setModified();
}

void XpressiveView::noiseWaveClicked() {
	m_expressionEditor->appendPlainText("randsv(t*srate,0)");
	Engine::getSong()->setModified();
}

void XpressiveView::moogSawWaveClicked()
{
	if (m_output_expr)
		m_expressionEditor->appendPlainText("moogsaww(integrate(f))");
	else
		m_expressionEditor->appendPlainText("moogsaww(t)");
	Engine::getSong()->setModified();
}
void XpressiveView::expWaveClicked()
{
	if (m_output_expr)
		m_expressionEditor->appendPlainText("expw(integrate(f))");
	else
		m_expressionEditor->appendPlainText("expw(t)");
	Engine::getSong()->setModified();
}

void XpressiveView::usrWaveClicked() {
	m_expressionEditor->setPlainText("");
	QString fileName = m_raw_graph->setWaveToUser();
	smoothChanged();
	Engine::getSong()->setModified();
}

QString XpressiveHelpView::s_helpText=
"<b>O1, O2</b> - Two output waves. Panning is controlled by PN1 and PN2.<br>"
"<b>W1, W2, W3</b> - Wave samples evaluated by expression. In these samples, t variable ranges [0,1).<br>"
"These waves can be used as functions inside the output waves (O1, O2). The wave period is 1.<br>"
"<h4>Available variables:</h4><br>"
"<b>t</b> - Time in seconds.<br>"
"<b>f</b> - Note's pitched frequency. Available only in the output expressions.<br>"
"<b>key</b> - Note's keyboard key. 0 denotes C-1, 60 denotes C4, 127 denotes G9. Available only in the output expressions.<br>"
"<b>bnote</b> - Base note. By default it is 69 which means A4, unless you change it.<br>"
"<b>srate</b> - Sample rate. In wave expression it returns the wave's number of samples.<br>"
"<b>tempo</b> - Song's Tempo. Available only in the output expressions.<br>"
"<b>v</b> - Note's volume. Note that the output is already multiplied by the volume. Available only in the output expressions.<br>"
"<b>rel</b> - Gives 0.0 while the key is held, and 1.0 after the key release. Available only in the output expressions.<br>"
"<b>trel</b> - Time after release. While the note is held, it gives 0.0. Afterwards, it starts counting seconds.<br>"
"The time it takes to shift from 0.0 to 1.0 after key release is determined by the REL knob<br>"
"<b>seed</b> - A random value that remains consistent in the lifetime of a single wave. Meant to be used with <b>randsv</b><br>"
"<b>A1, A2, A3</b> - General purpose knobs. You can reference them only in O1 and O2. In range [-1,1].<br>"
"<h4>Available functions:</h4><br>"
"<b>W1, W2, W3</b> - As mentioned before. You can reference them only in O1 and O2.<br>"
"<b>cent(x)</b> - Gives pow(2,x/1200), so you can multiply it with the f variable to pitch the frequency.<br>"
"100 cents equals one semitone<br>"
"<b>semitone(x)</b> - Gives pow(2,x/12), so you can multiply it with the f variable to pitch the frequency.<br>"
"<b>last(n)</b> - Gives you the last n'th evaluated sample. In O1 and O2 it keeps a whole second. Thus the argument n must be in the range [1,srate], or else, it will return 0.<br>"
"<b>integrate(x)</b> - Integrates x by delta t (It sums values and divides them by sample rate).<br>"
"If you use notes with automated frequency, you should use:<br>"
"sinew(integrate(f)) instead of sinew(t*f)<br>"
"<b>randv(x)</b> - A random vector. Each cell is reference by an integer index in the range [0,2^31]<br>"
"Each evaluation of an expression results in different random vector.<br>"
"Although, it remains consistent in the lifetime of a single wave.<br>"
"If you want a single random values you can use randv(0),randv(1)... <br>"
"and every reference to randv(a) will give you the same value."
"If you want a random wave you can use randv(t*srate).<br>"
"Each random value is in the range [-1,1).<br>"
"<b>randsv(x,seed)</b> - works exactly like randv(x),<br>"
"except that it lets you to select the seed manualy,<br>"
"if you want to try different random values and make it consistent in each evaluation.<br>"
"<b>sinew(x)</b> - A sine wave with period of 1 (In contrast to real sine wave which have a period of 2*pi).<br>"
"<b>trianglew(x)</b> - A triangle wave with period of 1.<br>"
"<b>squarew(x)</b> - A square wave with period of 1.<br>"
"<b>saww(x)</b> - A saw wave with period of 1.<br>"
"<b>clamp(min_val,x,max_val)</b> - If x is in range of (min_val,max_val) it returns x. Otherwise if it's greater than max_val it returns max_val, else returns min_val.<br>"
"<b>abs, sin, cos, tan, cot, asin, acos, atan, atan2, sinh, cosh, tanh, asinh, acosh, atanh, sinc, "
"hypot, exp, log, log2, log10, logn, pow, sqrt, min, max, floor, ceil, round, trunc, frac, "
"avg, sgn, mod, etc. are also available.</b><br>"
"<b>Operands + - * / % ^ &gt; &lt; &gt;= &lt;= == != &amp; | are also available.</b><br>"
"<b>Amplitude Modulation</b> - W1(t*f)*(1+W2(t*f))<br>"
"<b>Ring Modulation</b> - W1(t * f)*W2(t * f)<br>"
"<b>Mix Modulation</b> - 0.5*( W1(t * f) + W2(t * f) )<br>"
"<b>Frequency Modulation</b> - [vol1]*W1( integrate( f + srate*[vol2]*W2( integrate(f) ) ) )<br>"
"<b>Phase Modulation</b> - [vol1]*W1( integrate(f) + [vol2]*W2( integrate(f) ) )<br>"
		;

XpressiveHelpView::XpressiveHelpView():QTextEdit(s_helpText)
{

#if QT_VERSION < 0x50C00
	// Workaround for a bug in Qt versions below 5.12,
	// where argument-dependent-lookup fails for QFlags operators
	// declared inside a namepsace.
	// This affects the Q_DECLARE_OPERATORS_FOR_FLAGS macro in Instrument.h
	// See also: https://codereview.qt-project.org/c/qt/qtbase/+/225348

	using ::operator|;

#endif

	setWindowTitle ( "Xpressive Help" );
	setTextInteractionFlags ( Qt::TextSelectableByKeyboard | Qt::TextSelectableByMouse );
	getGUI()->mainWindow()->addWindowedWidget( this );
	parentWidget()->setAttribute( Qt::WA_DeleteOnClose, false );
	parentWidget()->setWindowIcon( PLUGIN_NAME::getIconPixmap( "logo" ) );
	parentWidget()->setFixedSize( 300, 500);
	
	// No maximize button
	Qt::WindowFlags flags = parentWidget()->windowFlags();
	flags &= ~Qt::WindowMaximizeButtonHint;
	parentWidget()->setWindowFlags( flags );
}

void XpressiveView::helpClicked() {
	XpressiveHelpView::getInstance()->show();

}


} // namespace gui

extern "C" {

// necessary for getting instance out of shared lib
PLUGIN_EXPORT Plugin * lmms_plugin_main(Model *m, void *) {
	return (new Xpressive(static_cast<InstrumentTrack *>(m)));
}

}


} // namespace lmms
