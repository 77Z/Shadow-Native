#include "Debug/Logger.hpp"
#include "ShadowIcons.hpp"
#include "imgui.h"
#include "imgui/imgui_utils.hpp"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "json_impl.hpp"
#include <cstdlib>
#include <exception>
#include <filesystem>
#include <string>
#define GL_SILENCE_DEPRECATION
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <GLES2/gl2.h>
#endif
#include <GLFW/glfw3.h> // Will drag system OpenGL headers
#include "ShadowWindow.hpp"
#include "imgui/theme.hpp"
#include <curl/curl.h>
#include "generated/autoconf.h"
#include "Configuration/EngineConfiguration.hpp"

#if CONFIG_USE_PROD_AUTH_SERVER
#define AUTH_SERVER_URL "https://nexus.77z.dev"
#else
#define AUTH_SERVER_URL "http://localhost:8787"
#endif

namespace Shadow::AXE {

namespace Account {

static void confirmedLogOut(ShadowWindow* window) {
	std::filesystem::remove(EngineConfiguration::getConfigDir() + "/a");
	window->close();
}

void onUpdateStatusBar(bool isInEditor, ShadowWindow* window) {
	using namespace ImGui;

	PushStyleColor(ImGuiCol_Button, IM_COL32(0, 0, 0, 0));
	PushStyleColor(ImGuiCol_ButtonHovered, IM_COL32(255, 255, 255, 100));

	if (Button(SHADOW_ICON_ACCOUNT)) {
		OpenPopup("AXEAccountPopup");
	}

	SetItemTooltip("Your 77Z Account");

	PopStyleColor(2);


	if (BeginPopup("AXEAccountPopup")) {
		TextUnformatted(SHADOW_ICON_77Z_LOGO);
		SameLine();
		TextUnformatted(EngineConfiguration::getUserName().c_str());
		Separator();
		TextUnformatted("Authenticated with:");
#if CONFIG_USE_PROD_AUTH_SERVER
		TextUnformatted("Production 77Z server");
#else
		TextUnformatted("Development Server");
#endif
		SetItemTooltip(AUTH_SERVER_URL);

		if (Button(SHADOW_ICON_SIGN_OUT " Log out and Exit")) {
			if (isInEditor)
				OpenPopup("Log out confirmation");
			else
				confirmedLogOut(window);
		}

		if (BeginPopupModal("Log out confirmation")) {
			TextWrapped("Save your work and log out through the Project Browser");

			if (Button("OKAY")) CloseCurrentPopup();
			EndPopup();
		}
		EndPopup();
	}
}

}

static std::string inpUsername = "";
static std::string inpPassword = "";

static std::string failureMessage = "";

static bool loggedIn = false;

size_t curlWriteCallback(void* contents, size_t size, size_t nmemb, std::string* s) {
	size_t totalSize = size * nmemb;
	s->append(static_cast<char*>(contents), totalSize);
	return totalSize;
}

void loginWithCreds() {
	CURLcode ret;
	CURL *hnd;
	struct curl_slist *slist1;
	std::string responseData;

	if (inpPassword.empty() || inpUsername.empty()) {
		failureMessage = "One or more fields are empty";
		return;
	}

	json authenticationCredentials;
	authenticationCredentials["username"] = inpUsername.c_str();
	authenticationCredentials["password"] = inpPassword.c_str();
	std::string credsAsString = authenticationCredentials.dump();

	slist1 = nullptr;
	slist1 = curl_slist_append(slist1, "Content-Type: application/json");

	hnd = curl_easy_init();
	curl_easy_setopt(hnd, CURLOPT_BUFFERSIZE, 102400L);
	curl_easy_setopt(hnd, CURLOPT_URL, AUTH_SERVER_URL "/api/v1/authenticate");
	curl_easy_setopt(hnd, CURLOPT_NOPROGRESS, 1L);
	curl_easy_setopt(hnd, CURLOPT_POSTFIELDS, credsAsString.c_str());
	curl_easy_setopt(hnd, CURLOPT_POSTFIELDSIZE_LARGE, (curl_off_t)credsAsString.length());
	curl_easy_setopt(hnd, CURLOPT_HTTPHEADER, slist1);
	curl_easy_setopt(hnd, CURLOPT_USERAGENT, "ShadowEngine/" SHADOW_VERSION_STRING);
	curl_easy_setopt(hnd, CURLOPT_MAXREDIRS, 50L);
#if BX_PLATFORM_LINUX | BX_PLATFORM_BSD
	curl_easy_setopt(hnd, CURLOPT_HTTP_VERSION, (long)CURL_HTTP_VERSION_2TLS);
#endif
	curl_easy_setopt(hnd, CURLOPT_CUSTOMREQUEST, "POST");
	curl_easy_setopt(hnd, CURLOPT_FTP_SKIP_PASV_IP, 1L);
	curl_easy_setopt(hnd, CURLOPT_TCP_KEEPALIVE, 1L);

	curl_easy_setopt(hnd, CURLOPT_SSL_VERIFYSTATUS, 1);
	curl_easy_setopt(hnd, CURLOPT_CAINFO, "./Resources/cert-bundle.crt");
	curl_easy_setopt(hnd, CURLOPT_CAPATH, "./Resources/cert-bundle.crt");

	curl_easy_setopt(hnd, CURLOPT_WRITEFUNCTION, curlWriteCallback);
	curl_easy_setopt(hnd, CURLOPT_WRITEDATA, &responseData);

	ret = curl_easy_perform(hnd);
	if (ret != CURLE_OK) {
		failureMessage = "Failed to connect to the authentication server: " + std::string(curl_easy_strerror(ret));
		goto cleanup;
	}

	// PRINT("Response: %s", responseData.c_str());

	try {
		json parsedResponse = json::parse(responseData);

		json storedAuthObject;
		storedAuthObject["K"] = std::string(parsedResponse["session"]);
		storedAuthObject["N"] = std::string(parsedResponse["prettyName"]);

		EngineConfiguration::setUserName(std::string(parsedResponse["prettyName"]));

		// Successful login because the server sent back valid JSON data, and
		// we got to this point, so some of the props must be true!
		loggedIn = true;

		JSON::dumpJsonToBson(storedAuthObject, EngineConfiguration::getConfigDir() + "/a");
	} catch (const std::exception& e) {
		ERROUT("%s", e.what());
		if (responseData == "0") {
			failureMessage = "Invalid credentials (did you type it correctly?)";
			goto cleanup;
		}

		if (responseData == "1") {
			failureMessage = "Type A Malformed request. This is not your fault, contact Vince.";
			goto cleanup;
		}

		if (responseData == "2") {
			failureMessage = "Type B Malformed request. This is not your fault, contact Vince.";
			goto cleanup;
		}

		failureMessage = "Unknown error!";
	}

	

cleanup:
	curl_easy_cleanup(hnd);
	hnd = nullptr;
	curl_slist_free_all(slist1);
	slist1 = nullptr;
	return;
}

/// Uses the session token stored on disk to verify with the server if the user
/// can be logged in again
bool alreadyLoggedIn() {
	std::string loc = EngineConfiguration::getConfigDir() + "/a";
	if (!std::filesystem::exists(loc)) return false;

	json authObject = JSON::readBsonFile(loc);

	const std::string sessionToken = authObject["K"];

	// submit session token to server

	CURLcode ret;
	CURL *hnd;
	struct curl_slist *slist1;
	std::string responseData;

	slist1 = nullptr;
	slist1 = curl_slist_append(slist1, "Content-Type: application/json");

	hnd = curl_easy_init();
	curl_easy_setopt(hnd, CURLOPT_BUFFERSIZE, 102400L);
	curl_easy_setopt(hnd, CURLOPT_URL, AUTH_SERVER_URL "/api/v1/verify");
	curl_easy_setopt(hnd, CURLOPT_NOPROGRESS, 1L);
	curl_easy_setopt(hnd, CURLOPT_POSTFIELDS, sessionToken.c_str());
	curl_easy_setopt(hnd, CURLOPT_POSTFIELDSIZE_LARGE, (curl_off_t)sessionToken.length());
	curl_easy_setopt(hnd, CURLOPT_HTTPHEADER, slist1);
	curl_easy_setopt(hnd, CURLOPT_USERAGENT, "ShadowEngine/" SHADOW_VERSION_STRING);
	curl_easy_setopt(hnd, CURLOPT_MAXREDIRS, 50L);
#if BX_PLATFORM_LINUX | BX_PLATFORM_BSD
	curl_easy_setopt(hnd, CURLOPT_HTTP_VERSION, (long)CURL_HTTP_VERSION_2TLS);
#endif
	curl_easy_setopt(hnd, CURLOPT_CUSTOMREQUEST, "POST");
	curl_easy_setopt(hnd, CURLOPT_FTP_SKIP_PASV_IP, 1L);
	curl_easy_setopt(hnd, CURLOPT_TCP_KEEPALIVE, 1L);

	curl_easy_setopt(hnd, CURLOPT_SSL_VERIFYSTATUS, 1);
	curl_easy_setopt(hnd, CURLOPT_CAINFO, "./Resources/cert-bundle.crt");
	curl_easy_setopt(hnd, CURLOPT_CAPATH, "./Resources/cert-bundle.crt");

	curl_easy_setopt(hnd, CURLOPT_WRITEFUNCTION, curlWriteCallback);
	curl_easy_setopt(hnd, CURLOPT_WRITEDATA, &responseData);

	ret = curl_easy_perform(hnd);
	if (ret != CURLE_OK) {
		ERROUT("Can't connect to auth server, the user will be logged out");
		failureMessage = "Authentication server didn't respond to log you in. You can try logging in again here";
		curl_easy_cleanup(hnd);
		curl_slist_free_all(slist1);
		return false;
	}

	if (responseData == "1") {
		PRINT("Existing session verified");
		EngineConfiguration::setUserName(std::string(authObject["N"]));
		curl_easy_cleanup(hnd);
		curl_slist_free_all(slist1);
		return true;
	}

	ERROUT("Malformed response data from server or the server doesn't agree to re-auth. The user will be logged out.");
	failureMessage = "Authentication server didn't agree to log you in again. Your sesssion probably just expired and you can log in again.";

	curl_easy_cleanup(hnd);
	curl_slist_free_all(slist1);
	return false;
}

bool startAXEAuthentication() {
	// Can I skip the user having to login?
	if (alreadyLoggedIn()) return true;

	// std::string loc = EngineConfiguration::getConfigDir() + "/a";
	// if (std::filesystem::exists(loc)) {
	// 	json authObject = JSON::readBsonFile(loc);
	// 	if ((std::string)authObject["K"])
	// 		return true;
	// }

	curl_global_init(CURL_GLOBAL_ALL);
#if defined(IMGUI_IMPL_OPENGL_ES2)
	// GL ES 2.0 + GLSL 100
	const char* glsl_version = "#version 100";
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
#elif defined(__APPLE__)
	// GL 3.2 + GLSL 150
	const char* glsl_version = "#version 150";
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // Required on Mac
#else
	// GL 3.0 + GLSL 130
	const char* glsl_version = "#version 130";
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	//glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only
#endif

	ShadowWindow window(720, 720, "Shadow Engine - AXE Authenticator", true, true);
	if (window.window == nullptr) return 1;
	glfwMakeContextCurrent(window.window);
	glfwSwapInterval(1); // Enable vsync

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

	ImGui::SetupTheme();

	ImGui_ImplGlfw_InitForOpenGL(window.window, true);
	ImGui_ImplOpenGL3_Init(glsl_version);

	// IMGUI_ENABLE_FREETYPE in imconfig to use Freetype for higher quality font rendering
	float sf = window.getContentScale();
	// float sf = 1.5;
	io.Fonts->AddFontFromFileTTF("./Resources/Inter-Medium.ttf", 16.0f * sf);
	ImFont* headingFont = io.Fonts->AddFontFromFileTTF("./Resources/Inter-Black.ttf", 40.0f * sf);
	ImGui::GetStyle().ScaleAllSizes(sf);

	while (!window.shouldClose()) {
		using namespace ImGui;

		if (loggedIn) window.close();

		window.pollEvents();

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		NewFrame();

		// DRAW

		ImGuiViewport* viewport = GetMainViewport();
		SetNextWindowPos(viewport->Pos);
		SetNextWindowSize(viewport->Size);
		SetNextWindowViewport(viewport->ID);
		PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(45.0f, 20.0f));
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse
			| ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove
			| ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus
			| ImGuiWindowFlags_MenuBar;

		PushStyleColor(ImGuiCol_MenuBarBg, IM_COL32(0, 0, 0, 255));

		Begin("RootWindow", nullptr, window_flags);

		PopStyleColor();

		PopStyleVar(3);

		PushFont(headingFont);
		TextCenter("77Z Account");
		PopFont();

		TextUnformatted("Username");
		InputText("##Username", &inpUsername);
		TextUnformatted("Password");
		InputText("##Password", &inpPassword, ImGuiInputTextFlags_Password);
		if (Button("Login")) { loginWithCreds(); }

		PushStyleColor(ImGuiCol_Text, IM_COL32(255, 0, 0, 255));
		TextWrapped("%s", failureMessage.c_str());
		PopStyleColor();

		End();

		// Rendering
		Render();
		int display_w, display_h;
		glfwGetFramebufferSize(window.window, &display_w, &display_h);
		glViewport(0, 0, display_w, display_h);
		glClearColor(0, 0, 0, 1);
		glClear(GL_COLOR_BUFFER_BIT);
		ImGui_ImplOpenGL3_RenderDrawData(GetDrawData());

		glfwSwapBuffers(window.window);
	}

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	curl_global_cleanup();

	window.shutdown();

	return loggedIn;
}

}
