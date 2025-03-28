﻿#include "pch.h"
#include "GuiBase.h"

std::string SettingsWindowBase::GetPluginName()
{
	return "OrganizeMyBakkesModGarage";
}

void SettingsWindowBase::SetImGuiContext(uintptr_t ctx)
{
	ImGui::SetCurrentContext(reinterpret_cast<ImGuiContext*>(ctx));
}

std::string PluginWindowBase::GetMenuName()
{
	return "OrganizeMyBakkesModGarage";
}

std::string PluginWindowBase::GetMenuTitle()
{
	return menuTitle_;
}

void PluginWindowBase::SetImGuiContext(uintptr_t ctx)
{
	ImGui::SetCurrentContext(reinterpret_cast<ImGuiContext*>(ctx));
}

bool PluginWindowBase::ShouldBlockInput()
{
	return ImGui::GetIO().WantCaptureMouse || ImGui::GetIO().WantCaptureKeyboard;
}

bool PluginWindowBase::IsActiveOverlay()
{
	return true;
}

void PluginWindowBase::OnOpen()
{
	isWindowOpen_ = true;
}

void PluginWindowBase::OnClose()
{
	isWindowOpen_ = false;
	
}

void PluginWindowBase::Render()
{

	ImGui::SetNextWindowSize(ImVec2(500, 400), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSizeConstraints(ImVec2(420, 200), ImVec2(FLT_MAX, FLT_MAX));

	if (!ImGui::Begin(menuTitle_.c_str(), &isWindowOpen_, ImGuiWindowFlags_None))
	{
		// Early out if the window is collapsed, as an optimization.
		ImGui::End();
		return;
	}

	RenderWindow();

	ImGui::End();

	if (!isWindowOpen_)
	{
		_globalCvarManager->executeCommand("togglemenu " + GetMenuName());
	}
}
