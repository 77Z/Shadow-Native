#include "imgui.h"
#include "imgui_canvas.h"
#include "imgui_internal.h"
#include <cstdint>

namespace ax::NodeEditor {
struct Config;
}

namespace ax::NodeEditor::Detail {

struct NodeBuilder;
struct Style;
struct LinkId;
struct PinId;
struct NodeId;
struct HintBuilder;
struct EditorAction;
struct CreateItemAction;
struct DeleteItemsAction;
struct ContextMenuAction;
struct ShortcutAction;
struct Object;
struct Node;
struct Link;

struct EditorContext {
	EditorContext(const ax::NodeEditor::Config* config = nullptr);
	~EditorContext();

	const Config& GetConfig() const { return m_Config; }

	Style& GetStyle() { return m_Style; }

	void Begin(const char* id, const ImVec2& size = ImVec2(0, 0));
	void End();

	bool DoLink(LinkId id, PinId startPinId, PinId endPinId, ImU32 color, float thickness);


	NodeBuilder& GetNodeBuilder() { return m_NodeBuilder; }
	HintBuilder& GetHintBuilder() { return m_HintBuilder; }

	EditorAction* GetCurrentAction() { return m_CurrentAction; }

	CreateItemAction& GetItemCreator() { return m_CreateItemAction; }
	DeleteItemsAction& GetItemDeleter() { return m_DeleteItemsAction; }
	ContextMenuAction& GetContextMenu() { return m_ContextMenuAction; }
	ShortcutAction& GetShortcut() { return m_ShortcutAction; }

	const ImGuiEx::CanvasView& GetView() const { return m_Canvas.View(); }
	const ImRect& GetViewRect() const { return m_Canvas.ViewRect(); }
	const ImRect& GetRect() const { return m_Canvas.Rect(); }

	void SetNodePosition(NodeId nodeId, const ImVec2& screenPosition);
	void SetGroupSize(NodeId nodeId, const ImVec2& size);
	ImVec2 GetNodePosition(NodeId nodeId);
	ImVec2 GetNodeSize(NodeId nodeId);

	void SetNodeZPosition(NodeId nodeId, float z);
	float GetNodeZPosition(NodeId nodeId);

	void MarkNodeToRestoreState(Node* node);
	void UpdateNodeState(Node* node);

	void RemoveSettings(Object* object);

	void ClearSelection();
	void SelectObject(Object* object);
	void DeselectObject(Object* object);
	void SetSelectedObject(Object* object);
	void ToggleObjectSelection(Object* object);
	bool IsSelected(Object* object);
	const vector<Object*>& GetSelectedObjects();
	bool IsAnyNodeSelected();
	bool IsAnyLinkSelected();
	bool HasSelectionChanged();
	uint64_t GetSelectionId() const { return m_SelectionId; }

	Node* FindNodeAt(const ImVec2& p);
	void FindNodesInRect(const ImRect& r, vector<Node*>& result, bool append = false, bool includeIntersecting = true);
	void FindLinksInRect(const ImRect& r, vector<Link*>& result, bool append = false);

	bool HasAnyLinks(NodeId nodeId) const;
	bool HasAnyLinks(PinId pinId) const;

	int BreakLinks(NodeId nodeId);
	int BreakLinks(PinId pinId);

	void FindLinksForNode(NodeId nodeId, vector<Link*>& result, bool add = false);

	bool PinHadAnyLinks(PinId pinId);

	ImVec2 ToCanvas(const ImVec2& point) const { return m_Canvas.ToLocal(point); }
	ImVec2 ToScreen(const ImVec2& point) const { return m_Canvas.FromLocal(point); }

	void NotifyLinkDeleted(Link* link);

	void Suspend(SuspendFlags flags = SuspendFlags::None);
	void Resume(SuspendFlags flags = SuspendFlags::None);
	bool IsSuspended();

	bool IsFocused();
	bool IsHovered() const;
	bool IsHoveredWithoutOverlapp() const;
	bool CanAcceptUserInput() const;

	void MakeDirty(SaveReasonFlags reason);
	void MakeDirty(SaveReasonFlags reason, Node* node);

	int CountLiveNodes() const;
	int CountLivePins() const;
	int CountLiveLinks() const;

	Pin*    CreatePin(PinId id, PinKind kind);
	Node*   CreateNode(NodeId id);
	Link*   CreateLink(LinkId id);

	Node*   FindNode(NodeId id);
	Pin*    FindPin(PinId id);
	Link*   FindLink(LinkId id);
	Object* FindObject(ObjectId id);

	Node*  GetNode(NodeId id);
	Pin*   GetPin(PinId id, PinKind kind);
	Link*  GetLink(LinkId id);

	Link* FindLinkAt(const ImVec2& p);

	template <typename T>
	ImRect GetBounds(const std::vector<T*>& objects)
	{
		ImRect bounds(FLT_MAX, FLT_MAX, -FLT_MAX, -FLT_MAX);

		for (auto object : objects)
			if (object->m_IsLive)
				bounds.Add(object->GetBounds());

		if (ImRect_IsEmpty(bounds))
			bounds = ImRect();

		return bounds;
	}

	template <typename T>
	ImRect GetBounds(const std::vector<ObjectWrapper<T>>& objects)
	{
		ImRect bounds(FLT_MAX, FLT_MAX, -FLT_MAX, -FLT_MAX);

		for (auto object : objects)
			if (object.m_Object->m_IsLive)
				bounds.Add(object.m_Object->GetBounds());

		if (ImRect_IsEmpty(bounds))
			bounds = ImRect();

		return bounds;
	}

	ImRect GetSelectionBounds() { return GetBounds(m_SelectedObjects); }
	ImRect GetContentBounds() { return GetBounds(m_Nodes); }

	ImU32 GetColor(StyleColor colorIndex) const;
	ImU32 GetColor(StyleColor colorIndex, float alpha) const;

	int GetNodeIds(NodeId* nodes, int size) const;

	void NavigateTo(const ImRect& bounds, bool zoomIn = false, float duration = -1)
	{
		auto zoomMode = zoomIn ? NavigateAction::ZoomMode::WithMargin : NavigateAction::ZoomMode::None;
		m_NavigateAction.NavigateTo(bounds, zoomMode, duration);
	}

	void RegisterAnimation(Animation* animation);
	void UnregisterAnimation(Animation* animation);

	void Flow(Link* link, FlowDirection direction);

	void SetUserContext(bool globalSpace = false);

	void EnableShortcuts(bool enable);
	bool AreShortcutsEnabled();

	NodeId GetHoveredNode()            const { return m_HoveredNode;             }
	PinId  GetHoveredPin()             const { return m_HoveredPin;              }
	LinkId GetHoveredLink()            const { return m_HoveredLink;             }
	NodeId GetDoubleClickedNode()      const { return m_DoubleClickedNode;       }
	PinId  GetDoubleClickedPin()       const { return m_DoubleClickedPin;        }
	LinkId GetDoubleClickedLink()      const { return m_DoubleClickedLink;       }
	bool   IsBackgroundClicked()                           const { return m_BackgroundClickButtonIndex >= 0; }
	bool   IsBackgroundDoubleClicked()                     const { return m_BackgroundDoubleClickButtonIndex >= 0; }
	ImGuiMouseButton GetBackgroundClickButtonIndex()       const { return m_BackgroundClickButtonIndex; }
	ImGuiMouseButton GetBackgroundDoubleClickButtonIndex() const { return m_BackgroundDoubleClickButtonIndex; }

	float AlignPointToGrid(float p) const
	{
		if (!ImGui::GetIO().KeyAlt)
			return p - ImFmod(p, 16.0f);
		else
			return p;
	}

	ImVec2 AlignPointToGrid(const ImVec2& p) const
	{
		return ImVec2(AlignPointToGrid(p.x), AlignPointToGrid(p.y));
	}

	ImDrawList* GetDrawList() { return m_DrawList; }

private:
	void LoadSettings();
	void SaveSettings();

	Control BuildControl(bool allowOffscreen);

	void ShowMetrics(const Control& control);

	void UpdateAnimations();

	Config              m_Config;

	ImGuiID             m_EditorActiveId;
	bool                m_IsFirstFrame;
	bool                m_IsFocused;
	bool                m_IsHovered;
	bool                m_IsHoveredWithoutOverlapp;

	bool                m_ShortcutsEnabled;

	Style               m_Style;

	vector<ObjectWrapper<Node>> m_Nodes;
	vector<ObjectWrapper<Pin>>  m_Pins;
	vector<ObjectWrapper<Link>> m_Links;

	vector<Object*>     m_SelectedObjects;

	vector<Object*>     m_LastSelectedObjects;
	uint64_t            m_SelectionId;

	Link*               m_LastActiveLink;

	vector<Animation*>  m_LiveAnimations;
	vector<Animation*>  m_LastLiveAnimations;

	ImGuiEx::Canvas     m_Canvas;
	bool                m_IsCanvasVisible;

	NodeBuilder         m_NodeBuilder;
	HintBuilder         m_HintBuilder;

	EditorAction*       m_CurrentAction;
	NavigateAction      m_NavigateAction;
	SizeAction          m_SizeAction;
	DragAction          m_DragAction;
	SelectAction        m_SelectAction;
	ContextMenuAction   m_ContextMenuAction;
	ShortcutAction      m_ShortcutAction;
	CreateItemAction    m_CreateItemAction;
	DeleteItemsAction   m_DeleteItemsAction;

	vector<AnimationController*> m_AnimationControllers;
	FlowAnimationController      m_FlowAnimationController;

	NodeId              m_HoveredNode;
	PinId               m_HoveredPin;
	LinkId              m_HoveredLink;
	NodeId              m_DoubleClickedNode;
	PinId               m_DoubleClickedPin;
	LinkId              m_DoubleClickedLink;
	int                 m_BackgroundClickButtonIndex;
	int                 m_BackgroundDoubleClickButtonIndex;

	bool                m_IsInitialized;
	Settings            m_Settings;

	ImDrawList*         m_DrawList;
	int                 m_ExternalChannel;
	ImDrawListSplitter  m_Splitter;
};
}