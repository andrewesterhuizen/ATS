#include "plugin.hpp"
#include "clock.h"

struct ATS : Module
{
	enum ParamIds
	{
		SPEED_PARAM,
		MODE_PARAM,
		FUNCTION_PARAM,
		NUM_PARAMS
	};
	enum InputIds
	{
		SPEED_INPUT,
		MODE_INPUT,
		FUNCTION_INPUT,
		NUM_INPUTS
	};
	enum OutputIds
	{
		TRIGGERA_OUTPUT,
		TRIGGERB_OUTPUT,
		TRIGGERC_OUTPUT,
		TRIGGERD_OUTPUT,
		TRIGGERE_OUTPUT,
		NUM_OUTPUTS
	};
	enum LightIds
	{
		TRIGGERA_LIGHT,
		TRIGGERB_LIGHT,
		TRIGGERC_LIGHT,
		TRIGGERD_LIGHT,
		TRIGGERE_LIGHT,
		NUM_LIGHTS
	};

	dsp::SchmittTrigger tapTempoInputTrigger;
	dsp::SchmittTrigger tapTempoButtonTrigger;
	dsp::PulseGenerator pulseGenerators[5];
	dsp::PulseGenerator ledPulseGenerators[5];

	int bpm = 120;
	float sampleAccumulator = 0;

	Clock ats;

	uint8_t clockState;

	bool waitingForNextTap = false;
	int samplesSinceTap = 0;

	float samplesPerTick = 0;

	ATS()
	{
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		configParam(SPEED_PARAM, 0.f, 1.f, 0.f, "");
		configParam(MODE_PARAM, 0.f, 1.f, 0.f, "");
		configParam(FUNCTION_PARAM, 0.f, 1.f, 0.f, "");

		ats.init(5);

		samplesPerTick = ((48000.0 / ((float)(bpm) / 60))) / 24;
	}

	void process(const ProcessArgs &args) override
	{
		samplesPerTick = ((args.sampleRate / ((float)(bpm) / 60))) / 24;
		// printf("args.sampleRate: %f\n", args.sampleRate);
		// printf("bpm: %d\n", bpm);

		// printf("samplesPerTick: %f\n", samplesPerTick);
		// printf("sampleAccumulator: %d\n", sampleAccumulator);

		sampleAccumulator++;
		if (sampleAccumulator >= samplesPerTick)
		{
			sampleAccumulator -= samplesPerTick;

			clockState = ats.tick();
			// printf("tick: ");

			// // check clock output state
			for (int x = 0; x < 5; x++)
			{
				// printf("%d", (clockState & (1 << x)) > 0);

				if (clockState & (1 << x))
				{
					pulseGenerators[x].trigger();
					ledPulseGenerators[x].trigger(0.3);
				}
			}

			// printf("\n");
		}

		// printf("output pulses\n");

		// output pulses
		for (int i = 0; i < 5; i++)
		{
			auto pulseState = pulseGenerators[i].process(args.sampleTime);
			auto ledPulseState = ledPulseGenerators[i].process(args.sampleTime);

			// printf("output: %d, state: %d \n", i, pulseState);

			if (pulseState)
			{
				outputs[i]
					.setVoltage(10);
			}
			else
			{
				outputs[i]
					.setVoltage(0);
			}

			if (ledPulseState)
			{
				lights[i].setBrightness(1);
			}
			else
			{
				lights[i].setBrightness(0);
			}
		}

		if (tapTempoButtonTrigger.process(params[SPEED_PARAM].getValue()) || tapTempoInputTrigger.process(inputs[SPEED_INPUT].getVoltage()))
		{
			printf("tap\n");
			if (waitingForNextTap)
			{
				printf("samplesSinceTap: %d\n", samplesSinceTap);
				float newBPM = 60 / (samplesSinceTap / args.sampleRate);
				printf("newBPM: %f\n", newBPM);

				bpm = newBPM;
				sampleAccumulator = 0;

				waitingForNextTap = false;
			}
			else
			{
				waitingForNextTap = true;
				samplesSinceTap = 0;
			}
		}

		if (waitingForNextTap)
		{
			samplesSinceTap++;
		}
	}
};

struct ATSWidget : ModuleWidget
{
	ATSWidget(ATS *module)
	{
		setModule(module);
		setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/ATS.svg")));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addParam(createParamCentered<TL1105>(mm2px(Vec(37.5, 24.25)), module, ATS::SPEED_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(37.5, 48.25)), module, ATS::MODE_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(37.5, 72.25)), module, ATS::FUNCTION_PARAM));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(13.0, 24.25)), module, ATS::SPEED_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(13.0, 48.25)), module, ATS::MODE_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(13.0, 72.25)), module, ATS::FUNCTION_INPUT));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(16.83, 93.0)), module, ATS::TRIGGERA_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(33.66, 93.0)), module, ATS::TRIGGERB_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(10.0, 108.5)), module, ATS::TRIGGERC_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(25.5, 108.5)), module, ATS::TRIGGERD_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(40.5, 108.5)), module, ATS::TRIGGERE_OUTPUT));

		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(19.33, 99.0)), module, ATS::TRIGGERA_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(36.13, 99.0)), module, ATS::TRIGGERB_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(12.5, 113.0)), module, ATS::TRIGGERC_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(27.25, 113.0)), module, ATS::TRIGGERD_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(43.0, 113.0)), module, ATS::TRIGGERE_LIGHT));
	}
};

Model *modelATS = createModel<ATS, ATSWidget>("ATS");