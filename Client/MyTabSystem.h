#pragma once
#include "imgui.h"
#include "imgui_internal.h"
#include "..\bootstrap\imgui_glfw3.h"
#include <vector>

namespace ImGui
{
	#ifdef IMGUI_DEFINE_MATH_OPERATORS
	static inline ImVec4 operator+(const ImVec4& lhs, const ImVec4& rhs) { return ImVec4(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z, lhs.w + rhs.w); }
	static inline ImVec4 operator*(const ImVec4& lhs, const ImVec4& rhs) { return ImVec4(lhs.x*rhs.x, lhs.y*rhs.y, lhs.z*rhs.z, lhs.w*rhs.w); }
	static inline ImVec4 operator/(const ImVec4& lhs, const ImVec4& rhs) { return ImVec4(lhs.x / rhs.x, lhs.y / rhs.y, lhs.z / rhs.z, lhs.w / rhs.w); }

	static inline ImVec4& operator+=(ImVec4& lhs, const ImVec4& rhs) { lhs.x += rhs.x; lhs.y += rhs.y; lhs.z += rhs.z; lhs.w += rhs.w; return lhs; }
	static inline ImVec4& operator-=(ImVec4& lhs, const ImVec4& rhs) { lhs.x -= rhs.x; lhs.y -= rhs.y; lhs.z -= rhs.z; lhs.w -= rhs.w; return lhs; }
	static inline ImVec4& operator*=(ImVec4& lhs, const ImVec4& rhs) { lhs.x *= rhs.x; lhs.y *= rhs.y; lhs.z *= rhs.z; lhs.w *= rhs.w; return lhs; }
	static inline ImVec4& operator/=(ImVec4& lhs, const ImVec4& rhs) { lhs.x /= rhs.x; lhs.y /= rhs.y; lhs.z /= rhs.z; lhs.w /= rhs.w; return lhs; }
	static inline ImVec2& operator+(const ImVec2& lhs, float rhs) { return ImVec2(lhs.x + rhs, lhs.y + rhs); }
	#endif
	/// User Colors & Style Extensions
	enum TabColours {
		TabBorder = 0,
		TabBorderShadow,
		TabNormal,
		TabHover,
		TabTitleTextNormal,
		TabTitleTextSelected,
		TabColoursCount
	};


	//Bitmask flags
	enum EdgeType : char {
		EDGE_NONE = 0,
		EDGE_LEFT = 1 << 0,
		EDGE_TOP = 1 << 1,
		EDGE_RIGHT = 1 << 2,
		EDGE_BOTTOM = 1 << 3
	};

	static void DrawPartialRect(const ImVec2 min, const ImVec2 max, const float rounding, const int roundingCorners, ImDrawList* drawList, const EdgeType edges, const ImU32 colour);

	struct UserAttributes
	{
		ImVec4 m_colours[TabColoursCount];
		UserAttributes();
	};

	static UserAttributes m_userAttribs;


	const ImU32 GetColourImU32(TabColours index, float alphaMul = 0.0f);



	//a class that represents a series of tabs structured in a row/bar
	class MyTabBar
	{
	public:
		MyTabBar(const char* label, const ImVec2 tabBarSize);
		~MyTabBar();

		const int GetActiveTab();

		void SetActiveTab(const int index);

		void DrawTabBarTop(const char* label);

		void DrawTabBarBottom();

	private:
		std::vector<const char*> m_tabTitles;
		std::vector<ImGuiID> m_tabHashes; //ImGUIID is a unique ID for all widgets
		int m_tabCount; //how many tabs in this bar
		int m_activeTab; //index of active tab

		std::string m_barTitle; //bar title (is this necessary?)
		ImVec2 m_tabBarSize;
		ImGuiID m_hash;

		int m_indexCounter; // Index counter, cleared every call to "BeginTabBar"
		ImVec2 m_upperLeftCorner;
		bool m_hasBeenInitialized;

		float m_cornerRounding; //for padding
		int m_newSelectedTab; //holds the value of a newly selected tab (wonder if this has to be a member variable)

		enum TabType : char { LEFTMOST_TAB, MIDDLE_TAB, RIGHTMOST_TAB }; //useful


	};


	class MyTabBarStack
	{

	};

	static MyTabBarStack m_tabBarStack;

	void TabBarBegin(const char* label, const ImVec2 size = ImVec2(0, 0));
}
