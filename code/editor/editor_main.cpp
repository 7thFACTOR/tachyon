#pragma once
#ifdef _WINDOWS
	#include "resource.h"
	#include <tchar.h>
#endif
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <wchar.h>
#include "base/cmdline_arguments.h"
#include "core/globals.h"
#include "core/module.h"
#include "base/variable.h"
#include "base/string.h"
#include "base/logger.h"
#include "core/resource_repository.h"
#include "core/application.h"
#include "editor_core/editor.h"
#include "logic/logic.h"
#include "graphics/graphics.h"
#include "base/task_scheduler.h"
#define HORUS_NO_BASIC_TYPES
//#include "horus.h"
#include <vector>

#ifdef _WINDOWS
	#include <windows.h>
#endif

#ifdef _LINUX
	#include <unistd.h>
#endif

using namespace base;
using namespace engine;
using namespace editor;

/*
using namespace hui;


char str[1500] = { 0 };
char str2[1500] = { 0 };
bool checks[100] = { false };
hui::Handle fntBig;
hui::Handle lenaImg;


struct MyViewHandler : hui::ViewHandler
{
	Handle moveIcon = 0;
	Handle playIcon = 0;
	Handle stopIcon = 0;
	Handle pauseIcon = 0;
	Handle horusLogo = 0;
	char* chooseColorPopup = "1";
	char* exitAppMsgBox = "2";
	char* moreInfoMsgBox = "3";
	Point chooseColorPopupPos;
	Point moreInfoBoxPos;

	hui::Color color = hui::Color::red();

	void onPopupsRender(Handle wnd)
	{
		return;
		if (hui::isPopupVisible(exitAppMsgBox))
		{
			auto msgBoxResult = hui::messageBox("pop", "Exit", "Exit this app?", hui::MessageBoxButtons::okCancel(), MessageBoxIcon::Question, 300);

			if (msgBoxResult.ok)
			{
				hui::quitApplication();
			}
			else if (msgBoxResult.cancel || msgBoxResult.closedByEscape)
			{
				hui::closePopup(exitAppMsgBox);
			}
		}

		if (hui::isPopupVisible(chooseColorPopup))
		{
			hui::beginPopup("a", 100, PopupPositionMode::Custom, chooseColorPopupPos);
			hui::gap(10);

			hui::pushTint(hui::Color::yellow());
			hui::labelCustomFont("Meet Lena", fntBig);
			hui::popTint();

			hui::image(lenaImg);
			hui::multilineLabel("Lenna or Lena is the name given to a standard test image widely used in the field of image processing since 1973.[1] It is a picture of Lena Söderberg, shot by photographer Dwight Hooker, cropped from the centerfold of the November 1972 issue of Playboy magazine.", HAlignType::Left);
			static bool ap = false;
			ap = hui::check("Approve her", ap);

			if (hui::button("More Info..."))
			{
				hui::showPopup(moreInfoMsgBox);
			}

			if (hui::button("Close") || hui::mustClosePopup(""))
			{
				printf("Close\n");
				hui::closePopup(chooseColorPopup);
			}

			if (hui::isPopupVisible(moreInfoMsgBox))
			{
				auto answer = hui::messageBox("", "Info", "More");

				if (answer.ok || answer.closedByEscape)
				{
					hui::closePopup(moreInfoMsgBox);
				}
			}

			hui::endPopup();
		}

		if (hui::wantsToQuit())
		{
			hui::cancelQuitApplication();
			hui::showPopup(exitAppMsgBox);
		}
	}

	void onViewPaneRender(Handle wnd, Handle viewPane, ViewId viewId) override
	{
		return;
		hui::Rect viewRect = hui::getViewPaneRect(viewPane);

		switch (viewId)
		{
		case 1:
		{
			hui::beginMenuBar();
			u32 newMenuItem = 0;
			u32 openMenuItem = 0;
			u32 saveMenuItem = 0;
			u32 saveAsMenuItem = 0;
			u32 exportMenuItem = 0;
			u32 exitMenuItem = 0;

			if (hui::beginMenu("File"))
			{
				newMenuItem = hui::menuItem("New...", "Ctrl+N");
				openMenuItem = hui::menuItem("Open...", "Ctrl+O");
				saveMenuItem = hui::menuItem("Save", "Ctrl+S");
				saveAsMenuItem = hui::menuItem("Save As...", "");
				hui::menuSeparator();
				exportMenuItem = hui::menuItem("Export...", "Ctrl+E");
				exitMenuItem = hui::menuItem("Exit", "Ctrl+Alt+X");
			}

			hui::endMenu();

			hui::endMenuBar();

			hui::gap(10);
			hui::button("Process assets for build");

			//MyCustomTabData* data = (MyCustomTabData*)hui::getActiveViewPaneTabUserData(viewPane);

			//if (!data)
			//{
			//	data = new MyCustomTabData();
			//	hui::setActiveViewPaneTabUserData(viewPane, data);
			//}

			//hui::textInput(data->str, 100);

			if (hui::button("Run all systems"))
			{
			}

			hui::textInput(str, 129);
			break;
		}

		// custom viewport pane
		case 2:
		{
			f32 cols[] = { 50, 50, 50, -1, 80 };
			hui::gap(5);
			hui::beginColumns(5, cols);
			hui::iconButton(playIcon);
			hui::nextColumn();
			hui::iconButton(stopIcon);
			hui::nextColumn();
			hui::iconButton(pauseIcon);
			hui::nextColumn();
			hui::textInput(str, 100, TextInputValueType::Text, "Type to filter scene");
			hui::nextColumn();
			hui::button("Search");
			hui::endColumns();

			// no margins for our viewport
			hui::pushPadding(0);

			auto viewRc = hui::beginViewport();
			printf("Custom viewport:%f %f %f %f\n", viewRc.x, viewRc.y, viewRc.width, viewRc.height);
			//glClearColor(0.3, .5, .8, 1);
			////glUseProgram(0);
			//glDisable(GL_TEXTURE_2D);
			//glClear(GL_COLOR_BUFFER_BIT);
			//glBegin(GL_LINES);
			//glVertex2d(-.9, -.9);
			//glVertex2d(.9, .9);
			//glVertex2d(-.9, .9);
			//glVertex2d(.9, -.9);
			//glEnd();

			//Point pts[] = {{10, 240}, {100, 240}, {120, 240}, {330, 240}, {350, 240}, {380, 100}, {400, 120}};
			//Point pts[] = { { 10, 240 },{ 100, 140 },{ 190, 240 }, {250, 240}};
			{
				hui::LineStyle ls;

				ls.width = 54;
				ls.color = hui::Color::yellow();
				hui::setLineStyle(ls);
				std::vector<hui::Point> pts;
				float x = 2;
				for (float u = 0; u < 3.14 * 2; u += 0.1)
				{
					pts.push_back(hui::Point(x += 15, 300 + sinf(u) * 100));

				}
				hui::drawPolyLine(pts.data(), pts.size());
			}

			{
				hui::LineStyle ls;

				ls.width = 3;
				ls.color = hui::Color::white();
				hui::setLineStyle(ls);
				std::vector<Point> pts;
				float x = 2;
				for (float u = 0; u < 3.14 * 2; u += 0.1)
				{
					pts.push_back(Point(x += 15, 370 + sinf(u) * 100));

				}
				hui::drawPolyLine(pts.data(), pts.size());
			}

			{
				hui::LineStyle ls;

				ls.width = 10;
				ls.color = hui::Color::green();
				hui::setLineStyle(ls);
				std::vector<Point> pts;
				float x = 2;
				for (float u = 0; u < 3.14 * 2; u += 0.1)
				{
					pts.push_back(Point(x += 15, 400 + sinf(u) * 100));

				}
				hui::drawPolyLine(pts.data(), pts.size());
			}

			{
				hui::LineStyle ls;

				ls.width = 8;
				ls.color = hui::Color::orange();
				hui::setLineStyle(ls);
				std::vector<Point> pts;
				float x = 2;
				for (float u = 0; u < 3.14 * 2; u += 0.1)
				{
					pts.push_back(Point(x += 15, 440 + sinf(u) * 100));

				}
				hui::drawPolyLine(pts.data(), pts.size());
			}

			{
				hui::LineStyle ls;

				ls.width = 6;
				ls.color = hui::Color::cyan();
				hui::setLineStyle(ls);
				std::vector<SplineControlPoint> pts;

				pts.resize(4);

				pts[0].center.x = 100;
				pts[0].center.y = 100;
				pts[0].rightTangent.x = 120;
				pts[0].rightTangent.y = 20;

				pts[1].center.x = 200;
				pts[1].center.y = 100;
				pts[1].leftTangent.x = 0;
				pts[1].leftTangent.y = 140;
				pts[1].rightTangent.x = 0;
				pts[1].rightTangent.y = 90;

				pts[2].center.x = 400;
				pts[2].center.y = 100;
				pts[2].leftTangent.x = 320;
				pts[2].leftTangent.y = 1420;
				pts[1].rightTangent.x = 20;
				pts[1].rightTangent.y = 190;

				pts[3].center.x = 700;
				pts[3].center.y = 150;
				pts[3].leftTangent.x = 320;
				pts[3].leftTangent.y = 520;

				hui::drawSpline(pts.data(), pts.size());

				hui::setLineStyle({ hui::Color::red(), 2 });
				hui::drawLine({ 50,40 }, { 500, 100 });
			}

			hui::endViewport();

			if (hui::isHovered())
			{
				hui::setMouseCursor(hui::MouseCursorType::CrossHair);
			}
			else
			{
				hui::setMouseCursor(hui::MouseCursorType::Arrow);
			}

			hui::popPadding();
			break;
		}
		case 3:
		{
			hui::Rect wrect = hui::beginCustomWidget();

			{
				hui::LineStyle ls;

				ls.width = 6;
				ls.color = hui::Color::cyan();
				hui::setLineStyle(ls);
				std::vector<SplineControlPoint> pts;

				pts.resize(4);

				pts[0].center.x = 100;
				pts[0].center.y = 100;
				pts[0].rightTangent.x = 120;
				pts[0].rightTangent.y = 20;

				pts[1].center.x = 200;
				pts[1].center.y = 100;
				pts[1].leftTangent.x = 0;
				pts[1].leftTangent.y = 140;
				pts[1].rightTangent.x = 0;
				pts[1].rightTangent.y = 90;

				pts[2].center.x = wrect.width;
				pts[2].center.y = 499;
				pts[2].leftTangent.x = 320;
				pts[2].leftTangent.y = 1420;
				pts[1].rightTangent.x = 20;
				pts[1].rightTangent.y = 190;

				pts[3].center.x = 700;
				pts[3].center.y = 150;
				pts[3].leftTangent.x = 320;
				pts[3].leftTangent.y = 520;

				hui::drawSpline(pts.data(), pts.size());

				hui::setLineStyle({ hui::Color::red(), 2 });
				hui::drawLine({ 50,40 }, { wrect.width, wrect.height });
			}

			hui::endCustomWidget();

			if (hui::isHovered())
			{
				hui::setMouseCursor(hui::MouseCursorType::Hand);
			}
			else
			{
				hui::setMouseCursor(hui::MouseCursorType::Arrow);
			}

			break;
		}
		case 4:
		case 5:
		case 6:
		case 7:
		case 8:
		{
			hui::gap(5);
			hui::labelCustomFont("General Settings", hui::getThemeFont("title"));
			static f32 scr[100] = { 0 };
			hui::image(horusLogo);
			hui::beginScrollView(hui::getRemainingViewPaneHeight(viewPane), scr[viewId]);
			hui::gap(5);

			// showing a message box

			if (hui::button("Exit"))
			{
				hui::showPopup(exitAppMsgBox);
			}

			//---

			static f32 prog = 0;
			hui::label("Processing the procedural Universe...");
			hui::progress(prog += 0.001f);

			hui::beginEqualColumns(2);

			hui::pushTint(color, hui::TintColorType::Body);

			if (hui::button("Choose color..."))
			{
				auto rect = hui::getLastWidgetRect();
				chooseColorPopupPos = { rect.x, rect.bottom() };
				hui::showPopup(chooseColorPopup);
			}

			hui::popTint(hui::TintColorType::Body);

			if (hui::button("Move"))
			{
			}

			hui::button("Rotate");
			hui::button("Scale");
			hui::nextColumn();
			hui::button("Subtract");
			hui::button("Extrude");
			hui::button("Trim edges");
			hui::button("Generate All edges");
			static f32 slideVal = 0;
			hui::endColumns();

			f32 colWidths[] = { 0.2f, 15, -1, 15, -1, 15, -1 };
			f32 colMinWidths[] = { 50, 0, 0, 0, 0, 0, 0 };
			hui::beginColumns(7, colWidths, colMinWidths);
			hui::label("Position"); hui::nextColumn();
			hui::label("X", HAlignType::Right); hui::nextColumn(); hui::textInput(str, 10); hui::nextColumn();
			hui::label("Y", HAlignType::Right); hui::nextColumn(); hui::textInput(str, 10); hui::nextColumn();
			hui::label("Z", HAlignType::Right); hui::nextColumn();	hui::textInput(str, 10);
			hui::endColumns();

			hui::beginColumns(7, colWidths, colMinWidths);
			hui::label("Rotation"); hui::nextColumn();
			hui::label("X", HAlignType::Right); hui::nextColumn(); hui::textInput(str, 10); hui::nextColumn();
			hui::label("Y", HAlignType::Right); hui::nextColumn(); hui::textInput(str, 10); hui::nextColumn();
			hui::label("Z", HAlignType::Right); hui::nextColumn(); hui::textInput(str, 10);
			hui::endColumns();

			hui::beginColumns(7, colWidths, colMinWidths);
			hui::label("Scale"); hui::nextColumn();
			hui::label("X", HAlignType::Right); hui::nextColumn(); hui::textInput(str, 10); hui::nextColumn();
			hui::label("Y", HAlignType::Right); hui::nextColumn(); hui::textInput(str, 10); hui::nextColumn();
			hui::label("Z", HAlignType::Right); hui::nextColumn(); hui::textInput(str, 10);
			hui::endColumns();

			hui::beginEqualColumns(3);
			hui::label("Far Plane:", HAlignType::Right);
			hui::nextColumn();
			hui::pushPadding(0);
			hui::sliderFloat(0, 1, slideVal, true, 0.2f);
			hui::popPadding();
			hui::nextColumn();
			static char valStr[20];
			sprintf(valStr, "%.2f", slideVal);
			hui::textInput(valStr, 10);
			slideVal = atof(valStr);
			hui::nextColumn();

			static f32 slideVal2 = 0;
			hui::beginEqualColumns(3);
			hui::label("X Scale:");
			hui::nextColumn();
			hui::pushPadding(0);
			hui::sliderFloat(0, 100, slideVal2);
			hui::popPadding();
			hui::nextColumn();
			static char valStr2[10];
			sprintf(valStr2, "%.2f", slideVal2);
			hui::textInput(valStr2, 10);
			slideVal2 = atof(valStr2);
			hui::nextColumn();

			static f32 slideVal3 = 0;
			hui::beginEqualColumns(3);
			hui::label("Y Scale:");
			hui::nextColumn();
			hui::pushPadding(0);
			hui::sliderFloat(0, 100, slideVal3);
			hui::popPadding();
			hui::nextColumn();
			static char valStr3[20];
			sprintf(valStr3, "%.2f", slideVal3);
			hui::textInput(valStr3, 10);
			slideVal3 = atof(valStr3);
			hui::nextColumn();

			hui::line();
			char some[33];
			sprintf(some, "%.2f", slideVal);
			hui::beginEqualColumns(2);
			hui::label("Value:");
			hui::nextColumn();
			hui::label(some);
			hui::nextColumn();

			static bool pnl1 = false;
			static bool pnl2 = false;
			static bool pnl3 = false;

			pnl1 = hui::panel("Options1", pnl1);

			if (pnl1)
			{
				hui::tooltip("And some amazing tooltip\nMultiline of course!");
				hui::textInput(str, 100, TextInputValueType::Text);
				hui::beginEqualColumns(2);
				checks[0] = hui::check("Show markers", checks[0]);
				checks[1] = hui::check("Show ships", checks[1]);
				checks[2] = hui::check("Show bullets", checks[2]);
				hui::nextColumn();
				checks[3] = hui::check("Always update", checks[3]);
				checks[4] = hui::check("Trim ribbon", checks[4]);
				hui::line();
				static int opt = 0;

				if (hui::radio("Use X axis", opt == 0))
					opt = 0;
				if (hui::radio("Use Y axis", opt == 1))
					opt = 1;
				if (hui::radio("Use Z axis", opt == 2))
					opt = 2;

				hui::nextColumn();
			}

			pnl2 = hui::panel("Options2", pnl2);

			if (pnl2)
			{
				hui::button("fasfdsd");
				hui::button("fasfdsd");
				hui::button("fasfdsd");
				hui::button("fasfdsd");
				hui::textInput(str, 100);
			}

			pnl3 = hui::panel("Options with scroll", pnl3);

			if (pnl3)
			{
				hui::textInput(str, 100);
				static f32 val = 0;
				hui::sliderFloat(0, 100, val);
				hui::button("Gogo1");
				hui::button("Gogo2");
				hui::button("Gogo3");
				hui::button("Gogo4");
				hui::button("Gogo5");
				hui::button("Gogo6");
				hui::button("Gogo7");
				hui::button("Gogo8");
				hui::button("Gogo last");
				hui::line();
			}

			scr[viewId] = hui::endScrollView();
			break;
		}
		}
	}

} myViewHandler;
*/

class EditorApp : public Application
{
public:
	void initialize()
	{
		setApplication(this);
		editor.initialize();
	}

	void shutdown()
	{
		editor.shutdown();
	}

	void update()
	{
		Application::update();
		//hui::updateDockingSystem(&myViewHandler);
		editor.update();
		getLogic().update();
		getResources().update();
		getGraphics().update();
		getTaskScheduler().reset();
		inputQueue.clearEvents();
	}

	Editor editor;
};

/*
void createMyDefaultViewPanes()
{
	auto myViewContainer = hui::createViewContainer(hui::getMainWindow());
	auto viewPane1 = hui::createViewPane(myViewContainer, hui::DockType::Left);
	hui::addViewPaneTab(viewPane1, "Assets", 0);
	hui::addViewPaneTab(viewPane1, "Console1", 1);
	hui::addViewPaneTab(viewPane1, "Console2", 1);
	hui::addViewPaneTab(viewPane1, "Scene", 2);
	auto viewPane2 = hui::createViewPane(myViewContainer, hui::DockType::Left);
	hui::addViewPaneTab(viewPane2, "Game", 3);
	auto viewPane3 = hui::createViewPane(myViewContainer, hui::DockType::Left);
	hui::addViewPaneTab(viewPane3, "Particles", 4);
	auto viewPane4 = hui::createViewPane(myViewContainer, hui::DockType::Bottom);
	hui::addViewPaneTab(viewPane4, "Properties", 5);
	auto viewPane5 = hui::createViewPane(myViewContainer, hui::DockType::Right);
	hui::addViewPaneTab(viewPane5, "World Entities And Other Creatures", 6);
}
*/

#if !defined(_CONSOLE) && defined(_WINDOWS)
int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
#else
int main(int argc, char* argv[])
#endif
{
	CommandLineArguments args;

#ifdef _WINDOWS
	args.parse(__argc, __argv);
#else
	args.parse(argc, argv);
#endif

	createEngineGlobals();
	getVariableRegistry().loadVariables("../config/engine.config");
	getVariableRegistry().loadVariables(args.getArgValue("config", "../config/app.config"));
	getVariableRegistry().loadVariables("../config/editor.config");
	getVariableRegistry().setVariableValue<i32>("sys_app_icon_id", IDI_EDITOR);
	getResources().addBundle("../data/engine");
	getResources().addBundle("../data/editor");

	if (!initializeEngine())
	{
		return false;
	}
	
	EditorApp editorApp;

	editorApp.initialize();
	// preload
	getResources().load(toResourceId("gpu_programs/default.gpu_program"));
	getResources().load(toResourceId("materials/default.material"));
	getResources().load(toResourceId("textures/default.png"));

	getModuleLoader().onLoadModules();

	//hui::initializeSDL("Tachyon Editor", { 0, 0, 800, 600 });
	//hui::setGlobalScale(1.0f);

	//auto theme = hui::loadTheme("../editor/default.theme", "../editor/fonts/arial.ttf");
	//fntBig = hui::createFont("../editor/fonts/arial.ttf", 20);

	//hui::setTheme(theme);
	//auto atlas = hui::createAtlas(2048, 2048);
	//auto moveIconId = hui::addAtlasSprite(atlas, hui::loadImage("../data/move_icon.png"));
	//auto playIconId = hui::addAtlasSprite(atlas, hui::loadImage("../data/play-icon.png"));
	//auto stopIconId = hui::addAtlasSprite(atlas, hui::loadImage("../data/stop-icon.png"));
	//auto pauseIconId = hui::addAtlasSprite(atlas, hui::loadImage("../data/pause-icon.png"));
	//auto horusLogoId = hui::addAtlasSprite(atlas, hui::loadImage("../data/horus.png"));
	//auto drawLineId = hui::addAtlasSprite(atlas, hui::loadImage("../data/drawline.png"));
	//auto lenaId = hui::addAtlasSprite(atlas, hui::loadImage("../data/lena.png"));
	//hui::packAtlas(atlas);
	//myViewHandler.moveIcon = hui::getAtlasSprite(atlas, moveIconId);
	//myViewHandler.playIcon = hui::getAtlasSprite(atlas, playIconId);
	//myViewHandler.stopIcon = hui::getAtlasSprite(atlas, stopIconId);
	//myViewHandler.pauseIcon = hui::getAtlasSprite(atlas, pauseIconId);
	//myViewHandler.horusLogo = hui::getAtlasSprite(atlas, horusLogoId);
	//lenaImg = hui::getAtlasSprite(atlas, lenaId);
	//bool wasLoaded = hui::loadViewContainersState("layout.hui");

	//hui::setLineAndFillAtlas(atlas);
	//hui::setLineSpriteId(drawLineId);

	//if (!wasLoaded)
	//{
	//	createMyDefaultViewPanes();
	//}

	editorApp.run();
	editorApp.shutdown();
	getModuleLoader().onUnloadModules();
	shutdownEngine();
	//hui::saveViewContainersState("layout.hui");
	//hui::shutdown();

	return 0;
}