// Console-OBS.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>

#include "C:/WORK/OBS/libobs/obs.h"
#include "C:/WORK/OBS/libobs/obs-module.h"

using namespace std;

// encoder enum callback
bool enum_encoder_proc(void* pData, obs_encoder_t* pEnc)
{
	if (!pEnc)
		return true;

	// check if this is video encoder
	video_t* pVidEnc(obs_encoder_video(pEnc));
	if (pVidEnc)
	{
		cout << "Video Encoder Found: " << obs_encoder_get_codec(pEnc) << endl;
		return true;
	}

	// check if this is audio encoder
	audio_t* pAudEnc(obs_encoder_audio(pEnc));
	if (pAudEnc)
	{
		cout << "Audio Encoder Found: " << obs_encoder_get_codec(pEnc) << endl;
		return true;
	}

	return true;
}

int main()
{
	bool bStarted(false);
	obs_encoder_t* pVideoEnc(nullptr);
	obs_encoder_t* pAudioEnc(nullptr);
	obs_service_t* pService(nullptr);
	obs_data* pTwitchData(nullptr);
	obs_output_t* pOutput(nullptr);

	try
	{
		// initialize the obs
		bStarted = obs_startup("en-US", nullptr, nullptr);
		if (!bStarted)
		{
			throw "obs_startup failed...";
		}

		// reset the audio
		obs_audio_info oai = { 0 };
		oai.samples_per_sec = 48000;
		oai.speakers = SPEAKERS_STEREO;

		if (!obs_reset_audio(&oai))
		{
			throw "obs_reset_audio failed...";
		}

		// set the data path for *.effect files
		// as without this obs_reset_video keeps getting failed
		obs_add_data_path("C:/WORK/OBS/libobs/data/");

		// reset the video
		// found the below video settings from OBS Studio log file
		obs_video_info ovi = { 0 };
		ovi.graphics_module = "libobs-d3d11";
		ovi.fps_num = 30;
		ovi.fps_den = 1;
		ovi.base_width = 1920;
		ovi.base_height = 1080;
		ovi.output_width = 1280;
		ovi.output_height = 720;
		ovi.output_format = VIDEO_FORMAT_NV12;
		ovi.gpu_conversion = true;
		ovi.colorspace = VIDEO_CS_709;
		ovi.range = VIDEO_RANGE_PARTIAL;
		ovi.scale_type = OBS_SCALE_BICUBIC;

		int iVideoStatus = obs_reset_video(&ovi);
		if (OBS_VIDEO_SUCCESS != iVideoStatus)
		{
			throw "obs_reset_video failed...";
		}

		// setting up the obs modules path 
		obs_add_module_path("C:/WORK/OBS/build_x64/rundir/Release/obs-plugins/64bit/", "C:/WORK/OBS/build_x64/rundir/Release/data/obs-plugins/");

		// set empty list to load all available modules
		obs_add_safe_module(nullptr);

		// loading all modules available at the above path
		// getting lots of errors and warning, dont know how to address that yet
		// but still able to load 17 modules
		obs_module_failure_info mfi = { 0 };
		obs_load_all_modules2(&mfi);

		// check for failed modules
		for (int i(0); i < mfi.count; ++i)
		{
			cout << "Warning: OBS Module failed to load: " << mfi.failed_modules[i] << endl;
		}

		// log all loaded modules
		obs_log_loaded_modules();

		// notifying that all modules are loaded
		obs_post_load_modules();

		// enumerate all encoders to get what we have on this system
		// somehow my callback is never invoked, dont know why???
		// so didn't spent too much time trying to fix this
		obs_enum_encoders(&enum_encoder_proc, nullptr);

		// creating video encoder
		// after various failed attempts finally "obs_x264" is found and created
		// get this encoder id from OBS Studio log file
		pVideoEnc = obs_video_encoder_create("obs_x264", "test_video_stream", nullptr, nullptr);
		if (!pVideoEnc)
		{
			throw "obs_video_encoder_create failed...";
		}

		// creating audio encoder
		// after various failed attempts finally "CoreAudio_AAC" is found and created
		// get this encoder id from OBS Studio log file
		pAudioEnc = obs_audio_encoder_create("CoreAudio_AAC", "test_audio_stream", nullptr, 0, nullptr);
		if (!pAudioEnc)
		{
			throw "obs_audio_encoder_create failed...";
		}

		// creating obs data with server url and key
		// created my first twitch account and got the below information
		pTwitchData = obs_data_create();
		obs_data_set_string(pTwitchData, "server", "rtmp://ingest.global-contribute.live-video.net/app/");
		obs_data_set_string(pTwitchData, "key", "live_1216564972_7rnZftYw3N4Ohh1q0YXsA5KFzHDK1f");

		// create twitch service
		pService = obs_service_create("rtmp_common", "Twitch", pTwitchData, nullptr);
		if (!pService)
		{
			throw "obs_service_create failed...";
		}

		// checking service values/parameters
		cout << "PROTOCOL: " << obs_service_get_protocol(pService) << endl;
		cout << "URL: " << obs_service_get_connect_info(pService, OBS_SERVICE_CONNECT_INFO_SERVER_URL) << endl;
		cout << "KEY: " << obs_service_get_connect_info(pService, OBS_SERVICE_CONNECT_INFO_STREAM_KEY) << endl;

		// create RTMP output
		pOutput = obs_output_create("rtmp_outupt", "test_stream", nullptr, nullptr);
		if (!pOutput)
		{
			throw "obs_output_create failed...";
		}

		// set the encoders and service to the output
		// got the below function call sequenece from OBS Studio source code

		obs_encoder_set_video(pVideoEnc, obs_get_video());
		obs_encoder_set_audio(pAudioEnc, obs_get_audio());

		obs_output_set_video_encoder(pOutput, pVideoEnc);
		obs_output_set_audio_encoder(pOutput, pAudioEnc, 0);

		obs_output_set_reconnect_settings(pOutput, 0, 0);

		obs_output_set_service(pOutput, pService);

		// start the output
		if (!obs_output_start(pOutput))
		{
			throw "obs_output_start failed...";
		}

		// should wait here for stream ???
		//Sleep(100000);

		// stop the output
		obs_output_stop(pOutput);
	}
	catch (const char* pError)
	{
		cout << "ERROR: " << pError << endl;
	}
	catch (...)
	{
		cout << "ERROR: Unknown" << endl;
	}

	// clean up section

	if (pOutput)
	{
		obs_output_release(pOutput);
	}

	if (pTwitchData)
	{
		obs_data_release(pTwitchData);
	}

	if (pService)
	{
		obs_service_release(pService);
	}

	if (pAudioEnc)
	{
		obs_encoder_release(pAudioEnc);
	}

	if (pVideoEnc)
	{
		obs_encoder_release(pVideoEnc);
	}

	if (bStarted)
	{
		obs_shutdown();
	}

	return 1;
}
