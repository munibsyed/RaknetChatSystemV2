#include "MyTabSystem.h"

ImGui::UserAttributes::UserAttributes()
{
	m_colours[TabNormal] = ImVec4(0.65f, 0.65f, 0.68f, 1.00f);
	m_colours[TabTitleTextNormal] = ImVec4(0.0f, 0.0f, 0.0f, 0.5f);
	m_colours[TabTitleTextSelected] = ImVec4(1.0f, 1.0f, 1.0f, 1.00f);
	m_colours[TabBorder] = m_colours[TabNormal] * ImVec4(1.15f, 1.15f, 1.15f, 1.0f);
	m_colours[TabBorderShadow] = m_colours[TabNormal] * ImVec4(0.65f, 0.65f, 0.65f, 1.0f);
	m_colours[TabHover] = m_colours[TabNormal] * ImVec4(1.15f, 1.15f, 1.15f, 1.0f);
}


ImGui::MyTabBar::MyTabBar(const char* label, const ImVec2 tabBarSize)
{
	//is label necessary? Find out
	m_tabCount = 0;
	m_activeTab = 0;
	m_indexCounter = 0;
	m_hash = ImHash(label, 0); //generate a unique hash for this tab bar
	m_tabBarSize = tabBarSize;
	m_upperLeftCorner = ImVec2(0, 0);
	m_hasBeenInitialized = false;
	m_cornerRounding = 0;
	m_newSelectedTab = -1;

}


ImGui::MyTabBar::~MyTabBar()
{
}

const int ImGui::MyTabBar::GetActiveTab()
{
	return m_activeTab;
}

void ImGui::MyTabBar::SetActiveTab(const int index)
{
	m_activeTab = index;
}

void ImGui::MyTabBar::DrawTabBarTop(const char * label)
{
	if (m_hasBeenInitialized == false)
	{
		return;
	}

	//Gather pointers and references
	ImGuiWindow* window = ImGui::GetCurrentWindow();
	ImDrawList* drawList = window->DrawList; //This is the low-level list of polygons that ImGui functions are filling.
	ImGuiDrawContext& drawContext = window->DC; //// Transient per-window data. Transient, i.e. reset at the beginning of each frame. Contains info such as mousePos, childWindows, etc.
	ImGuiStyle& style = ImGui::GetStyle();

	std::string labelStr = label;
	m_barTitle = labelStr.substr(0, labelStr.find_first_of('#'));
	if (m_barTitle.length() > 0)
	{
		//title of bar hovers above it
		ImGui::Text(std::string("\t" + m_barTitle).c_str()); //%s is probably unnecessary
	}

	//Get some geometric data
	const ImVec2 padding = style.WindowPadding;
	const ImVec2 framePadding = style.FramePadding;
	ImVec2 cursorPos = drawContext.CursorPos;
	const float spaceToEachTab = (window->Size.x - 2 * padding.x) / (float)m_tabCount;

	//Compile time constants
	static constexpr const float rounding = 6.0f;
	// This is the border shrink in px. Not sure why it needs to be 1, but it works. Adjust to your border size
	static constexpr const float shrink = 1.0f;

	const float m_tabHeight = CalcTextSize(m_tabTitles[0]).y + (framePadding.y * 2);

	//Is there any need to have separate variables for selectedTabOffset and offset, etc
	float selectedTabOffset = 0;
	TabType	selectedTabType; //leftmost, middle or rightmost
	ImVec2 selectedTabExpands;
	EdgeType selectedShadowEdges = EDGE_NONE;
	int selectedTabIndex = 0;

	//store minimum/maximum x value for clipping rect
	//window->pos.x is left most point on the window
	const float maxX = window->Pos.x + window->Size.x - padding.x;
	const float minX = window->Pos.x + padding.x;

	if (m_activeTab > m_tabCount - 1)
	{
		m_activeTab = m_tabCount - 1;
	}

	//index of the selected tab
	m_newSelectedTab = -1;

	//draw the tabs
	for (int i = 0; i < m_tabCount; i++)
	{
		//Calculate offset
		const float offset = i * spaceToEachTab;
		const bool isActiveTab = (m_activeTab == i);
		ImVec2 expands;
		if (isActiveTab == false)
		{
			if (i == 0)
				expands = ImVec2(0, 2);
			else if (i == m_tabCount - 1)
				expands = ImVec2(1, 0);
			else
				expands = ImVec2(1, 2);

			//think this is the expanse of the current tab to draw (xl to xr)
			const float xLeft = offset - expands.x + shrink;
			const float xRight = offset + spaceToEachTab - shrink + expands.y;

			const ImRect boundingBox = ImRect(ImVec2(cursorPos + ImVec2(xLeft, 0)), ImVec2(cursorPos + ImVec2(xRight, m_tabHeight)));
			bool hovered;
			bool held;
			bool isPressed = ButtonBehavior(boundingBox, m_tabHashes[i], &hovered, &held);
			if (held == true) //if this tab was held down (pressed), then this is the new tab
			{
				m_newSelectedTab = i;
			}

			ImU32 colour;
			if (hovered == true)
			{
				colour = GetColourImU32(TabHover);
			}
			else
			{
				colour = GetColourImU32(TabNormal);
			}

			//Draw background rect (DECIDE WHETHER TO USE ROUNDING OR NOT)
			drawList->AddRectFilled(boundingBox.Min, boundingBox.Max, colour, rounding, -1 | 1);

			//Draw the border (TO IMPLEMENT)
			// _drawPartialRect(bb.Min, bb.Max,rounding, ImGuiCorner_TopLeft | ImGuiCorner_TopRight, dl, (_EdgeType) (EDGE_LEFT | EDGE_RIGHT | EDGE_TOP), GetColorU32(ImGuiUserCol_TabBorderShadow));

			//draw the title within the tab
			const ImVec2 textSize = CalcTextSize(m_tabTitles[i]);
			const ImVec2 textPos = cursorPos + ImVec2(offset + ((xRight - xLeft) - textSize.x) / 2.0f, ((textSize.y - framePadding.y*2.0f) / 2.0f));
			drawList->AddText(textPos, GetColourImU32(TabTitleTextNormal), m_tabTitles[i]);
		}

		else //if we are on the active tab
		{
			selectedTabOffset = offset;
			selectedTabIndex = i;
			if (i == 0)
			{
				selectedTabExpands = ImVec2(0, rounding);
				selectedShadowEdges = EDGE_RIGHT;
			}
			else if (i == m_tabCount - 1)
			{
				selectedTabExpands = ImVec2(rounding, 0);
				selectedShadowEdges = EDGE_LEFT;
			}
			else
			{
				selectedTabExpands = ImVec2(rounding, rounding);
				selectedShadowEdges = (EdgeType)(EDGE_LEFT | EDGE_RIGHT);
			}
		}
	}
	//DO WE REALLY HAVE TO REPEAT ALL THIS? see if there is a way to make this code more compact
	//what are we drawing here? the same thing?
	const float xLeft = selectedTabOffset - selectedTabExpands.x + shrink;
	const float xRight = selectedTabOffset + spaceToEachTab - shrink + selectedTabExpands.y;
	const ImRect boundingBox = ImRect(ImVec2(cursorPos + ImVec2(xLeft, 0)), ImVec2(cursorPos + ImVec2(xRight, m_tabHeight)));
	// Draw the selected tab on top of everything else
	drawList->AddRectFilled(boundingBox.Min, boundingBox.Max, GetColourImU32(TabNormal), rounding, -1 | 1);

	drawList->AddRectFilled(boundingBox.Min, boundingBox.Max, ImColor(1.0f, 1.0f, 1.0f, 0.35f), rounding, -1 | 1);
	// Draw the border
	//_drawPartialRect(bb.Min, bb.Max, rounding, ImGuiCorner_TopLeft | ImGuiCorner_TopRight, dl, (_EdgeType)(EDGE_LEFT | EDGE_RIGHT | EDGE_TOP), GetColorU32(ImGuiUserCol_TabBorderShadow), true, selected_shadow_edges);
	// Draw the text
	const ImVec2 textSize = CalcTextSize(m_tabTitles[selectedTabIndex]);
	const ImVec2 textPos = cursorPos + ImVec2(selectedTabOffset + ((xRight - xLeft) - textSize.x) / 2.0f - selectedTabExpands.x, ((textSize.y - framePadding.y*2.0f) / 2.0f));
	drawList->AddText(textPos + ImVec2(1, 1), GetColourImU32(TabTitleTextNormal), m_tabTitles[selectedTabIndex]); // Shadow
	drawList->AddText(textPos, GetColourImU32(TabTitleTextSelected), m_tabTitles[selectedTabIndex]);

	drawContext.CursorPos += ImVec2(0, m_tabHeight + padding.y); // Add all the extra height used above.
	m_upperLeftCorner = drawContext.CursorPos - ImVec2(0, padding.y);
	Indent(padding.x);
	m_cornerRounding = rounding;
	//minX and maxX refers to the window itself	
	ImGui::PushClipRect(ImVec2(minX, boundingBox.Max.y), ImVec2(maxX, window->ClipRect.Max.y), false);
}	

void ImGui::MyTabBar::DrawTabBarBottom()
{
}

void ImGui::DrawPartialRect(const ImVec2 min, const ImVec2 max, const float rounding, const int roundingCorners, ImDrawList * drawList, const EdgeType edges, const ImU32 colour)
{
	float r = rounding;
	r = ImMin(r, std::abs(max.x - min.x) * (((roundingCorners&(1 | 2)) == (1 | 2)) || ((roundingCorners&(4 | 8)) == (4 | 8)) ? 0.5f : 1.0f) - 1.0f);
	r = ImMin(r, std::abs(max.y - min.y) * (((roundingCorners&(1 | 8)) == (1 | 8)) || ((roundingCorners&(2 | 4)) == (2 | 4)) ? 0.5f : 1.0f) - 1.0f);

	//min is upper left of bounding box
	//max is bottom right of bounding box

	//this will never be true, at least with the values being passed in

	//IF CORNERS ARE NOT ROUND, THEN EVERYTHING WILL BE DRAWN WITH STRAIGHT LINES USING PathStroke()
	if (r <= 0.0f || roundingCorners == 0)
	{
		//my guess is that passing in (EdgeType) EDGE_TOP | EDGE_LEFT, etc is essentially like creating a new enum value that is somewhere between all the others
		//using a logical and is a way to find out which enum values are within this new type
		if (edges & EDGE_TOP)
		{
			drawList->PathLineTo(min); //start at upper left
			drawList->PathLineTo(ImVec2(max.x, min.y)); //end at top right
			drawList->PathStroke(colour, false);
		}

		if (edges & EDGE_RIGHT)
		{
			drawList->PathLineTo(ImVec2(max.x, min.y));
			drawList->PathLineTo(max);
			drawList->PathStroke(colour, false);
		}

		if (edges & EDGE_BOTTOM)
		{
			drawList->PathLineTo(ImVec2(min.x, max.y));
			drawList->PathLineTo(max);
			drawList->PathStroke(colour, false);
		}

		if (edges & EDGE_LEFT)
		{
			drawList->PathLineTo(min);
			drawList->PathLineTo(ImVec2(min.x, max.y));
			drawList->PathStroke(colour, false);
		}
	}
	else
	{
		const float radius0 = (roundingCorners & 1) ? r : 0.0f; //00000001
		const float radius1 = (roundingCorners & 2) ? r : 0.0f; //00000010
		const float radius2 = (roundingCorners & 4) ? r : 0.0f; //00000100
		const float radius3 = (roundingCorners & 8) ? r : 0.0f; //00001000

		if (edges & EDGE_TOP)
		{
			drawList->PathArcToFast(ImVec2(min.x + radius0, min.y + radius0), radius0, 6, 9); //clock coordinates, I think it starts at East
			drawList->PathArcToFast(ImVec2(max.x - radius1, min.y + radius1), radius1, 9, 12);
			drawList->PathStroke(colour, false);
		}

		if (edges & EDGE_RIGHT)
		{
			drawList->PathArcToFast(ImVec2(max.x - radius1, min.y + radius1), radius1, 9, 12);
			drawList->PathArcToFast(ImVec2(max.x - radius2, max.y - radius2), radius2, 0, 3);
			drawList->PathStroke(colour, false);
		}

		if (edges & EDGE_BOTTOM)
		{
			drawList->PathArcToFast(ImVec2(max.x - radius2, max.y - radius2), radius2, 0, 3);
			drawList->PathArcToFast(ImVec2(min.x + radius3, max.y - radius3), radius3, 3, 6);
			drawList->PathStroke(colour, false, 1.0f);
		}

		if (edges & EDGE_LEFT)
		{
			drawList->PathArcToFast(ImVec2(min.x + radius3, max.y - radius3), radius3, 3, 6);
			drawList->PathArcToFast(ImVec2(min.x + radius0, min.y + radius0), radius0, 6, 9);
			drawList->PathStroke(colour, false);
		}

		//removed shadow stuff
	}
}

const ImU32 ImGui::GetColourImU32(TabColours index, float alphaMul)
{
	ImVec4 colour = m_userAttribs.m_colours[index];
	if (alphaMul > 0.0f)
		colour.w *= GImGui->Style.Alpha * alphaMul;
	return ColorConvertFloat4ToU32(colour);
}

void ImGui::TabBarBegin(const char * label, const ImVec2 size)
{

}

