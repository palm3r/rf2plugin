#pragma once

#include <Windows.h>
#include <string>
#include <vector>
#include <deque>
#include <mutex>

#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>

#include "InternalsPlugin.hpp"

namespace rf2 { class plugin; }

// implement these functions
const char* Rf2PluginName();
rf2::plugin* Rf2PluginCreate();

namespace rf2 {

	// Session identifier
	enum class session_type {
		testday = 0,
		p1, p2, p3, p4,
		q1, q2, q3, q4,
		warmup,
		r1, r2, r3, r4,
	};

	// Base class for rFactor2 plugins
	class plugin : public InternalsPluginV07 {
	public:
		plugin(std::string const& name)
			: pluginName_(name)
			, isServer_(false)
			, trackLoaded_(false)
			, sessionStarted_(false)
		{
		}

		virtual ~plugin() {}

		// Plugin name
		std::string const& PluginName() const { return pluginName_; }

		// Is plugin running on dedicated server?
		bool isServer() const { return isServer_; }

		// UserData directory
		std::string const& UserDataDir() const { return userDataDir_; }

		// CustomPluginOptions.JSON filename
		//std::string const& PluginOptionsJson() const { return pluginOptionsJson_; }

		// Latest results file
		std::string const& LatestResultsFile() const { return latestResultsFile_; }

		// Read configuration from default INI file
		// (rFactor\Plugins\PLUGIN\PLUGIN.ini)
		template <typename T>
		T ReadConfig(std::string const& section, std::string const& key, T const& defval = T())
		{
			return ReadConfig<T>(pluginName_, section, key, defval);
		}

		// Read configuration from specified INI file
		// (rFactor\Plugins\PLUGIN\NAME.ini)
		template <typename T>
		T ReadConfig(std::string const& name, std::string const& section,
			std::string const& key, T const& defval = T())
		{
			std::vector<std::string> v = {
				(boost::format(".\\Plugins\\%1%\\%2%.ini") % pluginName_ % name).str(),
				(boost::format(".\\Plugins\\%1%\\%2%.ini") % pluginName_ % pluginName_).str(),
			};
			for (auto const& file : v) {
				std::string value;
				value = ReadConfigString(file, section, key);
				if (!value.empty())
					return boost::lexical_cast<T>(value);
			}
			return defval;
		}

		// Write configuration to default INI file
		// (rFactor\Plugins\PLUGIN\PLUGIN.ini)
		template <typename T>
		bool WriteConfig(std::string const& section, std::string const& key, T const& value)
		{
			return WriteConfig<T>(pluginName_, section, key, value);
		}

		// Write configuration to specified INI file
		// (rFactor\Plugins\PLUGIN\NAME.ini)
		template <typename T>
		bool WriteConfig(std::string const& name, std::string const& section,
			std::string const& key, T const& value)
		{
			return WriteConfigString(
				(boost::format(".\\Plugins\\%1%\\%2%.ini") % pluginName_ % name).str(),
				section, key, std::to_string(value));
		}

		// Display message to the players (as chat message)
		template <typename... Args>
		void DisplayMessage(std::string const& format, Args&&... args)
		{
			std::lock_guard<std::mutex> lock(mutex_);
			messages_.push_back(strformat(format, forward<Args>(args)...));
		}

	protected:
		// Called when track is loaded (Weekend started)
		virtual void OnTrackLoaded(std::string const& track, double length) {}

		// Called when session is started
		// - type: session identifier
		// - duration: session duration (in seconds)
		// - laps: max laps
		virtual void OnSessionStarted(session_type type, double duration, int laps) {}

		virtual void OnStartup(long version) {}
		virtual void OnShutdown() {}
		virtual void OnLoad() {}
		virtual void OnUnload() {}
		virtual void OnStartSession() {}
		virtual void OnEndSession() {}
		virtual void OnEnterRealtime() {}
		virtual void OnExitRealtime() {}
		virtual bool OnWantsScoringUpdates() { return false; }
		virtual void OnUpdateScoring(const ScoringInfoV01 &info) {}
		virtual long OnWantsTelemetryUpdates() { return 0; }
		virtual void OnUpdateTelemetry(const TelemInfoV01 &info) {}
		virtual bool OnWantsGraphicsUpdates() { return false; }
		virtual void OnUpdateGraphics(const GraphicsInfoV01 &info) {}
		virtual bool OnRequestCommentary(CommentaryRequestInfoV01 &info) { return false; }
		virtual bool OnHasHardwareInputs() { return false; }
		virtual void OnUpdateHardware(const double fDT) {}
		virtual void OnEnableHardware() {}
		virtual void OnDisableHardware() {}
		virtual bool OnCheckHWControl(const char* const controlName, double &fRetVal) { return false; }
		virtual bool OnForceFeedback(double &forceValue) { return false; }
		virtual void OnError(const char* const msg) {}
		virtual void OnSetPhysicsOptions(PhysicsOptionsV01 &options) {}
		virtual unsigned char OnWantsToViewVehicle(CameraControlInfoV01 &camControl) { return 0; }
		virtual void OnUpdateGraphics(const GraphicsInfoV02 &info) {}
		virtual bool OnWantsToDisplayMessage(MessageInfoV01 &info) { return false; }
		virtual void OnSetEnvironment(const EnvironmentInfoV01 &info) {}
		virtual void OnInitScreen(const ScreenInfoV01 &info) {}
		virtual void OnUninitScreen(const ScreenInfoV01 &info) {}
		virtual void OnDeactivateScreen(const ScreenInfoV01 &info) {}
		virtual void OnReactivateScreen(const ScreenInfoV01 &info) {}
		virtual void OnRenderScreenBeforeOverlays(const ScreenInfoV01 &info) {}
		virtual void OnRenderScreenAfterOverlays(const ScreenInfoV01 &info) {}
		virtual void OnPreReset(const ScreenInfoV01 &info) {}
		virtual void OnPostReset(const ScreenInfoV01 &info) {}
		virtual bool OnInitCustomControl(CustomControlInfoV01 &info) { return false; }
		virtual bool OnWantsWeatherAccess() { return false; }
		virtual bool OnAccessWeather(double trackNodeSize, WeatherControlInfoV01 &info) { return false; }
		virtual void OnThreadStarted(long type) {}
		virtual void OnThreadStopping(long type) {}
		virtual bool OnGetCustomVariable(long i, CustomVariableV01 &var) { return false; }
		virtual void OnAccessCustomVariable(CustomVariableV01 &var) {}
		virtual void OnGetCustomVariableSetting(CustomVariableV01 &var, long i, CustomSettingV01 &setting) {}
		virtual bool OnWantsMultiSessionRulesAccess() { return false; }
		virtual bool OnAccessMultiSessionRules(MultiSessionRulesV01 &info) { return false; }
		virtual bool OnWantsTrackRulesAccess() { return false; }
		virtual bool OnAccessTrackRules(TrackRulesV01 &info) { return false; }
		virtual bool OnWantsPitMenuAccess() { return false; }
		virtual bool OnAccessPitMenu(PitMenuV01 &info) { return false; }

	private:
		std::string pluginName_;
		bool isServer_;
		bool trackLoaded_;
		bool sessionStarted_;

		std::string userDataDir_;
		std::string pluginOptionsJson_;
		std::string latestResultsFile_;

		std::deque<std::string> messages_;
		mutable std::mutex mutex_;

		void Startup(long version)
		{
			char path[_MAX_PATH];
			GetModuleFileName(0, path, sizeof(path));
			isServer_ = _stricmp(path, "rFactor2 Dedicated.exe") == 0;
			OnStartup(version);
		}

		void Shutdown()
		{
			OnShutdown();
		}

		void Load()
		{
			trackLoaded_ = true;
			OnLoad();
		}

		void Unload()
		{
			OnUnload();
		}

		void StartSession()
		{
			sessionStarted_ = true;
			OnStartSession();
		}

		void EndSession()
		{
			OnEndSession();
		}

		void EnterRealtime()
		{
			OnEnterRealtime();
		}

		void ExitRealtime()
		{
			OnExitRealtime();
		}

		bool WantsScoringUpdates()
		{
			return OnWantsScoringUpdates();
		}

		void UpdateScoring(const ScoringInfoV01 &info)
		{
			if (trackLoaded_) {
				trackLoaded_ = false;
				OnTrackLoaded(info.mTrackName, info.mLapDist);
			}
			if (sessionStarted_) {
				sessionStarted_ = false;
				OnSessionStarted(static_cast<session_type>(info.mSession), info.mEndET, info.mMaxLaps);
			}
			OnUpdateScoring(info);
		}

		long WantsTelemetryUpdates()
		{
			return OnWantsTelemetryUpdates();
		}

		void UpdateTelemetry(const TelemInfoV01 &info)
		{
			OnUpdateTelemetry(info);
		}

		bool WantsGraphicsUpdates()
		{
			return OnWantsGraphicsUpdates();
		}

		void UpdateGraphics(GraphicsInfoV01 const& info)
		{
			OnUpdateGraphics(info);
		}

		bool RequestCommentary(CommentaryRequestInfoV01 &info)
		{
			return OnRequestCommentary(info);
		}

		bool HasHardwareInputs()
		{
			return OnHasHardwareInputs();
		}

		void UpdateHardware(const double fDT)
		{
			OnUpdateHardware(fDT);
		}

		void EnableHardware()
		{
			OnEnableHardware();
		}

		void DisableHardware()
		{
			OnDisableHardware();
		}

		bool CheckHWControl(const char* const controlName, double &fRetVal)
		{
			return OnCheckHWControl(controlName, fRetVal);
		}

		bool ForceFeedback(double &forceValue)
		{
			return OnForceFeedback(forceValue);
		}

		void Error(const char* const msg)
		{
			OnError(msg);
		}

		void SetPhysicsOptions(PhysicsOptionsV01 &options)
		{
			OnSetPhysicsOptions(options);
		}

		unsigned char WantsToViewVehicle(CameraControlInfoV01 &camControl)
		{
			return OnWantsToViewVehicle(camControl);;
		}

		void UpdateGraphics(const GraphicsInfoV02 &info)
		{
			OnUpdateGraphics(info);
		}
		
		bool WantsToDisplayMessage(MessageInfoV01 &info)
		{
			std::lock_guard<std::mutex> lock(mutex_);
			if (!messages_.empty())
			{
				auto msg = messages_.front();
				messages_.pop_front();
				memset(info.mText, 0, sizeof(info.mText));
				sprintf_s(info.mText, msg.c_str());
				info.mDestination = isServer_ ? 1 : 0;
				info.mTranslate = 0;
				return true;
			}
			return OnWantsToDisplayMessage(info);
		}

		void SetEnvironment(const EnvironmentInfoV01 &info)
		{
			userDataDir_ = info.mPath[0];
			pluginOptionsJson_ = info.mPath[1];
			latestResultsFile_ = info.mPath[2];
			OnSetEnvironment(info);
		}

		void InitScreen(const ScreenInfoV01 &info)
		{
			OnInitScreen(info);
		}

		void UninitScreen(const ScreenInfoV01 &info)
		{
			OnUninitScreen(info);
		}

		void DeactivateScreen(const ScreenInfoV01 &info)
		{
			OnDeactivateScreen(info);
		}

		void ReactivateScreen(const ScreenInfoV01 &info)
		{
			OnReactivateScreen(info);
		}

		void RenderScreenBeforeOverlays(const ScreenInfoV01 &info)
		{
			OnRenderScreenBeforeOverlays(info);
		}

		void RenderScreenAfterOverlays(const ScreenInfoV01 &info)
		{
			OnRenderScreenAfterOverlays(info);
		}

		void PreReset(const ScreenInfoV01 &info)
		{
			OnPreReset(info);
		}

		void PostReset(const ScreenInfoV01 &info)
		{
			OnPostReset(info);
		}

		bool InitCustomControl(CustomControlInfoV01 &info)
		{
			return OnInitCustomControl(info);
		}

		bool WantsWeatherAccess()
		{
			return OnWantsWeatherAccess();
		}

		bool AccessWeather(double trackNodeSize, WeatherControlInfoV01 &info)
		{
			return OnAccessWeather(trackNodeSize, info);
		}

		void ThreadStarted(long type)
		{
			OnThreadStarted(type);
		}

		void ThreadStopping(long type)
		{
			OnThreadStopping(type);
		}

		bool GetCustomVariable(long i, CustomVariableV01 &var)
		{
			return OnGetCustomVariable(i, var);
		}

		void AccessCustomVariable(CustomVariableV01 &var)
		{
			OnAccessCustomVariable(var);
		}

		void GetCustomVariableSetting(CustomVariableV01 &var, long i, CustomSettingV01 &setting)
		{
			OnGetCustomVariableSetting(var, i, setting);
		}

		bool WantsMultiSessionRulesAccess()
		{
			return OnWantsMultiSessionRulesAccess();
		}

		bool AccessMultiSessionRules(MultiSessionRulesV01 &info)
		{
			return OnAccessMultiSessionRules(info);
		}

		bool WantsTrackRulesAccess()
		{
			return OnWantsTrackRulesAccess();
		}

		bool AccessTrackRules(TrackRulesV01 &info)
		{
			return OnAccessTrackRules(info);
		}
		
		bool WantsPitMenuAccess()
		{
			return OnWantsPitMenuAccess();
		}

		bool AccessPitMenu(PitMenuV01 &info)
		{
			return OnAccessPitMenu(info);
		}

		std::string ReadConfigString(std::string const& file,
			std::string const& section, std::string const& key)
		{
			for (int size = 1024; size < 1024 * 1024; size *= 2) {
				std::vector<char> buf(size, '\0');
				auto r = GetPrivateProfileString(section.c_str(), key.c_str(),
					"", &buf[0], static_cast<DWORD>(buf.size()), file.c_str());
				if (r == 0)
					return "";
				if (r != buf.size() - 1)
					return std::string(&buf[0]);
			}
			return "";
		}

		bool WriteConfigString(std::string const& file,
			std::string const& section, std::string const& key, std::string const& value)
		{
			return WritePrivateProfileString(section.c_str(),
				key.c_str(), value.c_str(), file.c_str()) != 0;
		}
	};

} // namespace rf2

extern "C" {
	__declspec(dllexport) const char* GetPluginName() { return Rf2PluginName(); }
	__declspec(dllexport) PluginObjectType GetPluginType() { return PluginObjectType::PO_INTERNALS; }
	__declspec(dllexport) int GetPluginVersion() { return 7; }
	__declspec(dllexport) PluginObject* CreatePluginObject() { return Rf2PluginCreate(); }
	__declspec(dllexport) void DestroyPluginObject(PluginObject *obj) { delete static_cast<rf2::plugin*>(obj); }
}
