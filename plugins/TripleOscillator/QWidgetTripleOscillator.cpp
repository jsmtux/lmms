#include "QWidgetTripleOscillator.h"

#include "widgets/AutomatableButton.h"
#include "widgets/Knob.h"
#include "widgets/PixmapButton.h"


namespace lmms {
namespace gui {

gui::InstrumentView* QWidgetTripleOscillator::createView( QWidget * _parent )
{
	return new gui::TripleOscillatorView( this, _parent );
}

class TripleOscKnob : public Knob
{
public:
	TripleOscKnob( IFloatAutomatableModel* _model, QWidget * _parent ) :
			Knob( knobStyled, _model, _parent )
	{
		setFixedSize( 28, 35 );
	}
};

// 82, 109


TripleOscillatorView::TripleOscillatorView( TripleOscillator * _instrument,
							QWidget * _parent ) :
	InstrumentViewImpl( _instrument, _parent, true )
{
	setAutoFillBackground( true );
	QPalette pal;
	pal.setBrush( backgroundRole(),
				PLUGIN_NAME::getIconPixmap( "artwork" ) );
	setPalette( pal );

	const int mod_x = 66;
	const int mod1_y = 58;
	const int mod2_y = 75;
	const int osc_y = 109;
	const int osc_h = 52;

	// TODO: clean rewrite using layouts and all that...
	auto pm_osc1_btn = new PixmapButton(this, MFact::create(false, this));
	pm_osc1_btn->move( mod_x, mod1_y );
	pm_osc1_btn->setActiveGraphic( PLUGIN_NAME::getIconPixmap(
								"pm_active" ) );
	pm_osc1_btn->setInactiveGraphic( PLUGIN_NAME::getIconPixmap(
							"pm_inactive" ) );
	pm_osc1_btn->setToolTip(tr("Modulate phase of oscillator 1 by oscillator 2"));

	auto am_osc1_btn = new PixmapButton(this, MFact::create(false, this));
	am_osc1_btn->move( mod_x + 35, mod1_y );
	am_osc1_btn->setActiveGraphic( PLUGIN_NAME::getIconPixmap(
								"am_active" ) );
	am_osc1_btn->setInactiveGraphic( PLUGIN_NAME::getIconPixmap(
							"am_inactive" ) );
	am_osc1_btn->setToolTip(tr("Modulate amplitude of oscillator 1 by oscillator 2"));

	auto mix_osc1_btn = new PixmapButton(this, MFact::create(false, this));
	mix_osc1_btn->move( mod_x + 70, mod1_y );
	mix_osc1_btn->setActiveGraphic( PLUGIN_NAME::getIconPixmap(
							"mix_active" ) );
	mix_osc1_btn->setInactiveGraphic( PLUGIN_NAME::getIconPixmap(
							"mix_inactive" ) );
	mix_osc1_btn->setToolTip(tr("Mix output of oscillators 1 & 2"));

	auto sync_osc1_btn = new PixmapButton(this, MFact::create(false, this));
	sync_osc1_btn->move( mod_x + 105, mod1_y );
	sync_osc1_btn->setActiveGraphic( PLUGIN_NAME::getIconPixmap(
							"sync_active" ) );
	sync_osc1_btn->setInactiveGraphic( PLUGIN_NAME::getIconPixmap(
							"sync_inactive" ) );
	sync_osc1_btn->setToolTip(tr("Synchronize oscillator 1 with "
							"oscillator 2" ) );

	auto fm_osc1_btn = new PixmapButton(this, MFact::create(false, this));
	fm_osc1_btn->move( mod_x + 140, mod1_y );
	fm_osc1_btn->setActiveGraphic( PLUGIN_NAME::getIconPixmap(
								"fm_active" ) );
	fm_osc1_btn->setInactiveGraphic( PLUGIN_NAME::getIconPixmap(
							"fm_inactive" ) );
	fm_osc1_btn->setToolTip(tr("Modulate frequency of oscillator 1 by oscillator 2"));

	m_mod1BtnGrp = new automatableButtonGroup( m_instrument->m_osc[0]->m_modulationAlgoModel, this );
	m_mod1BtnGrp->addButton( pm_osc1_btn );
	m_mod1BtnGrp->addButton( am_osc1_btn );
	m_mod1BtnGrp->addButton( mix_osc1_btn );
	m_mod1BtnGrp->addButton( sync_osc1_btn );
	m_mod1BtnGrp->addButton( fm_osc1_btn );

	auto pm_osc2_btn = new PixmapButton(this, MFact::create(false, this));
	pm_osc2_btn->move( mod_x, mod2_y );
	pm_osc2_btn->setActiveGraphic( PLUGIN_NAME::getIconPixmap(
								"pm_active" ) );
	pm_osc2_btn->setInactiveGraphic( PLUGIN_NAME::getIconPixmap(
							"pm_inactive" ) );
	pm_osc2_btn->setToolTip(tr("Modulate phase of oscillator 2 by oscillator 3"));

	auto am_osc2_btn = new PixmapButton(this, MFact::create(false, this));
	am_osc2_btn->move( mod_x + 35, mod2_y );
	am_osc2_btn->setActiveGraphic( PLUGIN_NAME::getIconPixmap(
								"am_active" ) );
	am_osc2_btn->setInactiveGraphic( PLUGIN_NAME::getIconPixmap(
							"am_inactive" ) );
	am_osc2_btn->setToolTip(tr("Modulate amplitude of oscillator 2 by oscillator 3"));

	auto mix_osc2_btn = new PixmapButton(this, MFact::create(false, this));
	mix_osc2_btn->move( mod_x + 70, mod2_y );
	mix_osc2_btn->setActiveGraphic( PLUGIN_NAME::getIconPixmap(
							"mix_active" ) );
	mix_osc2_btn->setInactiveGraphic( PLUGIN_NAME::getIconPixmap(
							"mix_inactive" ) );
	mix_osc2_btn->setToolTip(tr("Mix output of oscillators 2 & 3"));

	auto sync_osc2_btn = new PixmapButton(this, MFact::create(false, this));
	sync_osc2_btn->move( mod_x + 105, mod2_y );
	sync_osc2_btn->setActiveGraphic( PLUGIN_NAME::getIconPixmap(
							"sync_active" ) );
	sync_osc2_btn->setInactiveGraphic( PLUGIN_NAME::getIconPixmap(
							"sync_inactive" ) );
	sync_osc2_btn->setToolTip(tr("Synchronize oscillator 2 with oscillator 3"));

	auto fm_osc2_btn = new PixmapButton(this, MFact::create(false, this));
	fm_osc2_btn->move( mod_x + 140, mod2_y );
	fm_osc2_btn->setActiveGraphic( PLUGIN_NAME::getIconPixmap(
								"fm_active" ) );
	fm_osc2_btn->setInactiveGraphic( PLUGIN_NAME::getIconPixmap(
							"fm_inactive" ) );
	fm_osc2_btn->setToolTip(tr("Modulate frequency of oscillator 2 by oscillator 3"));

	m_mod2BtnGrp = new automatableButtonGroup( m_instrument->m_osc[1]->m_modulationAlgoModel, this );
	m_mod2BtnGrp->addButton( pm_osc2_btn );
	m_mod2BtnGrp->addButton( am_osc2_btn );
	m_mod2BtnGrp->addButton( mix_osc2_btn );
	m_mod2BtnGrp->addButton( sync_osc2_btn );
	m_mod2BtnGrp->addButton( fm_osc2_btn );


	for( int i = 0; i < NUM_OF_OSCILLATORS; ++i )
	{
		int knob_y = osc_y + i * osc_h;

		// setup volume-knob
		auto vk = new Knob(knobStyled, m_instrument->m_osc[i]->m_volumeModel, this);
		vk->setVolumeKnob( true );
		vk->setFixedSize( 28, 35 );
		vk->move( 6, knob_y );
		vk->setHintText( tr( "Osc %1 volume:" ).arg(
							 i+1 ), "%" );

		// setup panning-knob
		Knob * pk = new TripleOscKnob( m_instrument->m_osc[i]->m_panModel, this );
		pk->move( 35, knob_y );
		pk->setHintText( tr("Osc %1 panning:").arg( i + 1 ), "" );

		// setup coarse-knob
		Knob * ck = new TripleOscKnob( m_instrument->m_osc[i]->m_coarseModel, this );
		ck->move( 82, knob_y );
		ck->setHintText( tr( "Osc %1 coarse detuning:" ).arg( i + 1 )
						 , " " + tr( "semitones" ) );

		// setup knob for left fine-detuning
		Knob * flk = new TripleOscKnob( m_instrument->m_osc[i]->m_fineLeftModel, this );
		flk->move( 111, knob_y );
		flk->setHintText( tr( "Osc %1 fine detuning left:" ).
						  arg( i + 1 ),
							" " + tr( "cents" ) );

		// setup knob for right fine-detuning
		Knob * frk = new TripleOscKnob( m_instrument->m_osc[i]->m_fineRightModel, this );
		frk->move( 140, knob_y );
		frk->setHintText( tr( "Osc %1 fine detuning right:" ).
						  arg( i + 1 ),
							" " + tr( "cents" ) );

		// setup phase-offset-knob
		Knob * pok = new TripleOscKnob( m_instrument->m_osc[i]->m_phaseOffsetModel, this );
		pok->move( 188, knob_y );
		pok->setHintText( tr( "Osc %1 phase-offset:" ).
						  arg( i + 1 ),
							" " + tr( "degrees" ) );

		// setup stereo-phase-detuning-knob
		Knob * spdk = new TripleOscKnob( m_instrument->m_osc[i]->m_stereoPhaseDetuningModel, this );
		spdk->move( 217, knob_y );
		spdk->setHintText( tr("Osc %1 stereo phase-detuning:" ).
						arg( i + 1 ),
							" " + tr( "degrees" ) );

		int btn_y = 96 + i * osc_h;

		auto sin_wave_btn = new PixmapButton(this, MFact::create(false, this));
		sin_wave_btn->move( 128, btn_y );
		sin_wave_btn->setActiveGraphic( PLUGIN_NAME::getIconPixmap(
							"sin_shape_active" ) );
		sin_wave_btn->setInactiveGraphic( PLUGIN_NAME::getIconPixmap(
							"sin_shape_inactive" ) );
		sin_wave_btn->setToolTip(
				tr( "Sine wave" ) );

		auto triangle_wave_btn = new PixmapButton(this, MFact::create(false, this));
		triangle_wave_btn->move( 143, btn_y );
		triangle_wave_btn->setActiveGraphic(
			PLUGIN_NAME::getIconPixmap( "triangle_shape_active" ) );
		triangle_wave_btn->setInactiveGraphic(
			PLUGIN_NAME::getIconPixmap( "triangle_shape_inactive" ) );
		triangle_wave_btn->setToolTip(
				tr( "Triangle wave") );

		auto saw_wave_btn = new PixmapButton(this, MFact::create(false, this));
		saw_wave_btn->move( 158, btn_y );
		saw_wave_btn->setActiveGraphic( PLUGIN_NAME::getIconPixmap(
							"saw_shape_active" ) );
		saw_wave_btn->setInactiveGraphic( PLUGIN_NAME::getIconPixmap(
							"saw_shape_inactive" ) );
		saw_wave_btn->setToolTip(
				tr( "Saw wave" ) );

		auto sqr_wave_btn = new PixmapButton(this, MFact::create(false, this));
		sqr_wave_btn->move( 173, btn_y );
		sqr_wave_btn->setActiveGraphic( PLUGIN_NAME::getIconPixmap(
						"square_shape_active" ) );
		sqr_wave_btn->setInactiveGraphic( PLUGIN_NAME::getIconPixmap(
						"square_shape_inactive" ) );
		sqr_wave_btn->setToolTip(
				tr( "Square wave" ) );

		auto moog_saw_wave_btn = new PixmapButton(this, MFact::create(false, this));
		moog_saw_wave_btn->move( 188, btn_y );
		moog_saw_wave_btn->setActiveGraphic(
			PLUGIN_NAME::getIconPixmap( "moog_saw_shape_active" ) );
		moog_saw_wave_btn->setInactiveGraphic(
			PLUGIN_NAME::getIconPixmap( "moog_saw_shape_inactive" ) );
		moog_saw_wave_btn->setToolTip(
				tr( "Moog-like saw wave" ) );

		auto exp_wave_btn = new PixmapButton(this, MFact::create(false, this));
		exp_wave_btn->move( 203, btn_y );
		exp_wave_btn->setActiveGraphic( PLUGIN_NAME::getIconPixmap(
							"exp_shape_active" ) );
		exp_wave_btn->setInactiveGraphic( PLUGIN_NAME::getIconPixmap(
							"exp_shape_inactive" ) );
		exp_wave_btn->setToolTip(
				tr( "Exponential wave" ) );

		auto white_noise_btn = new PixmapButton(this, MFact::create(false, this));
		white_noise_btn->move( 218, btn_y );
		white_noise_btn->setActiveGraphic(
			PLUGIN_NAME::getIconPixmap( "white_noise_shape_active" ) );
		white_noise_btn->setInactiveGraphic(
			PLUGIN_NAME::getIconPixmap( "white_noise_shape_inactive" ) );
		white_noise_btn->setToolTip(
				tr( "White noise" ) );

		auto uwb = new PixmapButton(this, MFact::create(false, this));
		uwb->move( 233, btn_y );
		uwb->setActiveGraphic( PLUGIN_NAME::getIconPixmap(
							"usr_shape_active" ) );
		uwb->setInactiveGraphic( PLUGIN_NAME::getIconPixmap(
							"usr_shape_inactive" ) );
		uwb->setToolTip(tr("User-defined wave"));

		connect( uwb, SIGNAL( doubleClicked() ),
				m_instrument->m_osc[i], SLOT( oscUserDefWaveDblClick() ) );

		auto uwt = new PixmapButton(this, m_instrument->m_osc[i]->m_useWaveTableModel);
		uwt->move( 110, btn_y );
		uwt->setActiveGraphic( PLUGIN_NAME::getIconPixmap(
							"wavetable_active" ) );
		uwt->setInactiveGraphic( PLUGIN_NAME::getIconPixmap(
							"wavetable_inactive" ) );
		uwt->setCheckable(true);
		uwt->setToolTip(tr("Use alias-free wavetable oscillators."));

		auto wsbg = new automatableButtonGroup(m_instrument->m_osc[i]->m_waveShapeModel, this);

		wsbg->addButton( sin_wave_btn );
		wsbg->addButton( triangle_wave_btn );
		wsbg->addButton( saw_wave_btn );
		wsbg->addButton( sqr_wave_btn );
		wsbg->addButton( moog_saw_wave_btn );
		wsbg->addButton( exp_wave_btn );
		wsbg->addButton( white_noise_btn );
		wsbg->addButton( uwb );
	}
}

}


extern "C"
{

// necessary for getting instance out of shared lib
Plugin * lmms_plugin_main( void* model, void * )
{
	return new gui::QWidgetTripleOscillator( static_cast<IInstrumentTrack *>( model ) );
}

}

}
