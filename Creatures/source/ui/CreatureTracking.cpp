#include "CreatureTracking.h"
#include <deque>
#include "WindowsHandler.h"

class CreatureTracker;

map<CreatureUniqueID, CreatureTracker> activeCreatureTrackers;

// Expired creature trackers to be deleted
vector<CreatureUniqueID> expiredCreatureTrackers;

// Vectors to be used by all active creature trackers to avoid per-creature-tracker memory allocations
vector<vec2> creatureTracker_BrainNodePositions;
vector<float> creatureTracker_PlotData;

class CreatureTracker
{
	CreatureUniqueID creatureID;
	CreatureData creatureSnapShot;
	WindowID windowID;

	bool cameraFollow;

	bool overlay_Halo;
	bool overlay_ForwardDir;
	bool overlay_RightDir;
	bool overlay_Eye;

	deque<float> plot_Life;
	deque<float> plot_Energy;
	deque<float> plot_Meat;

	void Show()
	{

		if (!WindowsHandler::BeginAndUpdate(windowID))
		{
			WindowsHandler::InactivateWindow();
			return;
		}

		ShowMisc();
		ShowBodyParams();
		ShowEyes();
		ShowBrain();

		WindowsHandler::InactivateWindow();
	}

	void ShowMisc()
	{
		float simToViewportScale = SimulationScaleToViewportScale(1.0);

		// Follow camera
		ImGui::Checkbox("Follow", &cameraFollow);
		if (cameraFollow)
		{
			Camera::InterpolatePositionTo(creatureSnapShot.pos, UI_CREATURE_TRACKER_CAMERA_ZOOM_BASED_FOLLOW_INTERPOLATION_RATE.value);
		}

		// Draw creature halo
		ImGui::Checkbox("Halo", &overlay_Halo);
		if (overlay_Halo)
		{
			ImGui::GetBackgroundDrawList()->AddCircle(
				SimulationSpaceToViewportSpace(creatureSnapShot.pos),
				simToViewportScale * creatureSnapShot.rad + UI_CREATURE_TRACKER_HALO_RADIUS_PIXEL_BIAS,
				IM_COL32(0, 255, 0, 100),
				UI_CREATURE_TRACKER_HALO_NUM_OF_SEGMENTS,
				UI_CREATURE_TRACKER_HALO_PIXEL_THICKNESS
			);
		}

		// Draw creature forward direction
		ImGui::Checkbox("Forward Direction", &overlay_ForwardDir);
		if (overlay_ForwardDir)
		{
			vec2 p1 = SimulationSpaceToViewportSpace(creatureSnapShot.pos);
			vec2 p2 = SimulationSpaceToViewportSpace(creatureSnapShot.pos + creatureSnapShot.forwardDir);
			vec2 p1Add = normalize(p2 - p1);
			p1Add *= UI_CREATURE_TRACKER_DIRECTION_PIXEL_DISTANCE;
			ImGui::GetBackgroundDrawList()->AddLine(
				p1,
				p1 + p1Add,
				IM_COL32(0, 255, 0, 100),
				UI_CREATURE_TRACKER_DEFAULT_LINE_PIXEL_THICKNESS
			);
		}

		// Draw creature right direction
		ImGui::Checkbox("Right Direction", &overlay_RightDir);
		if (overlay_RightDir)
		{
			vec2 p1 = SimulationSpaceToViewportSpace(creatureSnapShot.pos);
			vec2 p2 = SimulationSpaceToViewportSpace(creatureSnapShot.pos + creatureSnapShot.rightDir);
			vec2 p1Add = normalize(p2 - p1);
			p1Add *= UI_CREATURE_TRACKER_DIRECTION_PIXEL_DISTANCE;
			ImGui::GetBackgroundDrawList()->AddLine(
				p1,
				p1 + p1Add,
				IM_COL32(0, 255, 0, 100),
				UI_CREATURE_TRACKER_DEFAULT_LINE_PIXEL_THICKNESS
			);
		}
	}


	void ShowBodyParams()
	{

		vec2 plotDimensions = vec2(ImGui::GetContentRegionAvail().x, 50);

		deque<float>::iterator it = plot_Life.begin();
		unsigned int plotSize = plot_Life.size();
		for (unsigned int i = 0; i < UI_CREATURE_TRACKER_PLOT_NUM_OF_VALUES; ++i)
		{
			float value = i < plotSize ? *it : 0.0;
			creatureTracker_PlotData[i] = value;
			it++;
		}
		ImGui::PlotLines("", creatureTracker_PlotData.data(), UI_CREATURE_TRACKER_PLOT_NUM_OF_VALUES, 0, "Life", 0, CREATURE_MAX_LIFE.value, plotDimensions);

		it = plot_Energy.begin();
		plotSize = plot_Energy.size();
		for (unsigned int i = 0; i < UI_CREATURE_TRACKER_PLOT_NUM_OF_VALUES; ++i)
		{
			float value = i < plotSize ? *it : 0.0;
			creatureTracker_PlotData[i] = value;
			it++;
		}
		ImGui::PlotLines("", creatureTracker_PlotData.data(), UI_CREATURE_TRACKER_PLOT_NUM_OF_VALUES, 0, "Energy", 0, CREATURE_MAX_ENERGY.value, plotDimensions);

		it = plot_Meat.begin();
		plotSize = plot_Meat.size();
		for (unsigned int i = 0; i < UI_CREATURE_TRACKER_PLOT_NUM_OF_VALUES; ++i)
		{
			float value = i < plotSize ? *it : 0.0;
			creatureTracker_PlotData[i] = value;
			it++;
		}
		ImGui::PlotLines("", creatureTracker_PlotData.data(), UI_CREATURE_TRACKER_PLOT_NUM_OF_VALUES, 0, "Meat", 0, CREATURE_MAX_MEAT.value, plotDimensions);
	}

	void ShowBrain()
	{
		ImDrawList* drawList = ImGui::GetWindowDrawList();
		if (ImGui::CollapsingHeader("Brain"))
		{

			vec2 brainPos = ImGui::GetCursorScreenPos();
			vec2 brainSize = ImGui::GetContentRegionAvail();

			float brainShrink = 15.0;
			brainPos.x += brainShrink;
			brainPos.y += brainShrink;
			brainSize.x -= brainShrink * 2.0;
			brainSize.y -= brainShrink * 2.0;
			
			vec2 nodePos = vec2(0, 0);
			float nodeDrawRadius = 10.0;

			unsigned int nodeIndex = 0;
			unsigned int linkIndex = 0;

			// Draw node links, collect node positions, calculate node draw radius

			unsigned int numOfLevels = creatureSnapShot.brainStructure[0];
			float spaceBetweenLevels = brainSize.x / (numOfLevels);

			for (unsigned int level = 1; level <= numOfLevels; ++level)
			{
				unsigned int numOfNodesInLevel = creatureSnapShot.brainStructure[level];
				float spaceBetweenNodes = brainSize.y / (numOfNodesInLevel);

				vec2 offset = vec2(spaceBetweenLevels / 2.0, spaceBetweenNodes / 2.0);

				for (unsigned int node = 1; node <= numOfNodesInLevel; ++node)
				{
					// Collect position
					vec2 nodeDrawPos = brainPos + offset + nodePos;
					creatureTracker_BrainNodePositions[nodeIndex] = nodeDrawPos;

					nodePos.y += spaceBetweenNodes;
					nodeIndex++;

					// Draw links
					if (level <= 1) continue;
					unsigned int numOfNodesInPrevLevel = creatureSnapShot.brainStructure[level - 1];
					unsigned int prevLevelIndexOffset = nodeIndex - node - numOfNodesInPrevLevel;
					for (unsigned int prevLevelNode = 0; prevLevelNode < numOfNodesInPrevLevel; ++prevLevelNode)
					{
						unsigned int prevNodeIndex = prevLevelIndexOffset + prevLevelNode;
						vec2 prevLevelNodeDrawPos = creatureTracker_BrainNodePositions[prevNodeIndex];

						float linkNormalizedWeight = creatureSnapShot.brainLinks[linkIndex] / UI_CREATURE_TRACKER_BRAIN_LINK_WEIGHT_MAX_ABS_NORMALIZER;
						float linkNormalizedActivation = creatureSnapShot.brainNodes[prevNodeIndex] * linkNormalizedWeight;		

						float linkColorR = UI_CREATURE_TRACKER_BRAIN_LINK_NEUTRAL_COLOR_R;
						float linkColorG = UI_CREATURE_TRACKER_BRAIN_LINK_NEUTRAL_COLOR_G;
						float linkColorB = UI_CREATURE_TRACKER_BRAIN_LINK_NEUTRAL_COLOR_B;
						float linkAlpha = abs(linkNormalizedActivation);
						float linkThickness = abs(linkNormalizedActivation) * UI_CREATURE_TRACKER_BRAIN_LINK_MAX_WIDTH;

						if (linkNormalizedActivation > 0.0)
						{
							drawList->AddLine(
								prevLevelNodeDrawPos,
								nodeDrawPos,
								ImColor(
									UI_CREATURE_TRACKER_BRAIN_LINK_POSITIVE_VALUE_COLOR_R,
									UI_CREATURE_TRACKER_BRAIN_LINK_POSITIVE_VALUE_COLOR_G,
									UI_CREATURE_TRACKER_BRAIN_LINK_POSITIVE_VALUE_COLOR_B,
									linkAlpha
								),
								linkThickness
							);
						}
						else if (linkNormalizedActivation < 0.0)
						{
							drawList->AddLine(
								prevLevelNodeDrawPos,
								nodeDrawPos,
								ImColor(
									UI_CREATURE_TRACKER_BRAIN_LINK_NEGATIVE_VALUE_COLOR_R,
									UI_CREATURE_TRACKER_BRAIN_LINK_NEGATIVE_VALUE_COLOR_G,
									UI_CREATURE_TRACKER_BRAIN_LINK_NEGATIVE_VALUE_COLOR_B,
									linkAlpha
								),
								linkThickness
							);
						}

						

						linkIndex++;
					}
				}
				nodePos.x += spaceBetweenLevels;
				nodePos.y = 0;
			}


			// Draw node backgrounds
			for (unsigned int i = 0; i < nodeIndex; ++i)
			{
				vec2 nodeDrawPos = creatureTracker_BrainNodePositions[i];
				drawList->AddCircleFilled(
					nodeDrawPos,
					nodeDrawRadius,
					IM_COL32(
						UI_CREATURE_TRACKER_BRAIN_NODE_BACKGROUND_COLOR_R,
						UI_CREATURE_TRACKER_BRAIN_NODE_BACKGROUND_COLOR_G,
						UI_CREATURE_TRACKER_BRAIN_NODE_BACKGROUND_COLOR_B,
						UI_CREATURE_TRACKER_BRAIN_NODE_ALPHA
					),
					UI_CREATURE_TRACKER_BRAIN_NODE_NUM_OF_SEGMENTS
				);
			}

			// Draw nodes activations
			for (unsigned int i = 0; i < nodeIndex; ++i)
			{
				vec2 nodeDrawPos = creatureTracker_BrainNodePositions[i];
				float nodeActivation = creatureSnapShot.brainNodes[i];
				
				if (nodeActivation < 0.0)
				{
					drawList->AddCircleFilled(
						nodeDrawPos,
						nodeDrawRadius * -nodeActivation,
						IM_COL32(
							UI_CREATURE_TRACKER_BRAIN_NEGATIVE_VALUE_COLOR_R,
							UI_CREATURE_TRACKER_BRAIN_NEGATIVE_VALUE_COLOR_G,
							UI_CREATURE_TRACKER_BRAIN_NEGATIVE_VALUE_COLOR_B,
							UI_CREATURE_TRACKER_BRAIN_NODE_ALPHA
						),
						UI_CREATURE_TRACKER_BRAIN_NODE_NUM_OF_SEGMENTS
					);
				}
				else
				{
					drawList->AddCircleFilled(
						nodeDrawPos,
						nodeDrawRadius * nodeActivation,
						IM_COL32(
							UI_CREATURE_TRACKER_BRAIN_POSITIVE_VALUE_COLOR_R,
							UI_CREATURE_TRACKER_BRAIN_POSITIVE_VALUE_COLOR_G,
							UI_CREATURE_TRACKER_BRAIN_POSITIVE_VALUE_COLOR_B,
							UI_CREATURE_TRACKER_BRAIN_NODE_ALPHA
						),
						UI_CREATURE_TRACKER_BRAIN_NODE_NUM_OF_SEGMENTS
					);
				}
			}
		}
	}

	void ShowEyes()
	{
		float simToViewportScale = SimulationScaleToViewportScale(1.0);

		if (ImGui::CollapsingHeader("Eye"))
		{
			
			
			ImGui::Checkbox("Show Eye Overlay", &overlay_Eye);
			
			if (ImGui::TreeNode("Eye Pupil Sightings"))
			{
				float barWidth = UI_CREATURE_TRACKER_EYE_PUPIL_SIGHTS_BAR_WIDTH;
				float barHeight = UI_CREATURE_TRACKER_EYE_PUPIL_SIGHTS_BAR_HEIGHT;
				vec2 barDimensions = vec2(barWidth, barHeight);

				ImGui::ProgressBar(creatureSnapShot.eyePupilSights[0], barDimensions, "Activation");
				ImGui::ProgressBar(creatureSnapShot.eyePupilSights[1], barDimensions, "Shield Prominence");
				ImGui::ProgressBar(creatureSnapShot.eyePupilSights[2], barDimensions, "Spike Prominence");
				ImGui::ProgressBar(creatureSnapShot.eyePupilSights[3], barDimensions, "Feeder Prominence");
				ImGui::ProgressBar(creatureSnapShot.eyePupilSights[4], barDimensions, "Forward Aim Prominence");
				ImGui::ProgressBar(creatureSnapShot.eyePupilSights[5], barDimensions, "Skin Hue Difference");
				ImGui::ProgressBar(creatureSnapShot.eyePupilSights[6], barDimensions, "Skin Lightness");
				ImGui::ProgressBar(creatureSnapShot.eyePupilSights[7], barDimensions, "Skin Saturation");
				ImGui::ProgressBar(creatureSnapShot.eyePupilSights[8], barDimensions, "Skin Pattern Difference");
				ImGui::ProgressBar(creatureSnapShot.eyePupilSights[9], barDimensions, "Life");
				ImGui::ProgressBar(creatureSnapShot.eyePupilSights[10], barDimensions, "Energy");
				ImGui::ProgressBar(creatureSnapShot.eyePupilSights[11], barDimensions, "Radius");

				ImGui::TreePop();
			}

			if (ImGui::TreeNode("Eye Cones Sightings"))
			{
				ImGui::TreePop();
			}

		}

		if (overlay_Eye)
		{
			// Draw eye probe area
			ImGui::GetBackgroundDrawList()->AddQuad(
				SimulationSpaceToViewportSpace(creatureSnapShot.pos + (creatureSnapShot.rightDir + creatureSnapShot.forwardDir) * CREATURE_EYE_MAX_PROBE_DISTANCE.value),
				SimulationSpaceToViewportSpace(creatureSnapShot.pos + (creatureSnapShot.rightDir - creatureSnapShot.forwardDir) * CREATURE_EYE_MAX_PROBE_DISTANCE.value),
				SimulationSpaceToViewportSpace(creatureSnapShot.pos + (-creatureSnapShot.rightDir - creatureSnapShot.forwardDir) * CREATURE_EYE_MAX_PROBE_DISTANCE.value),
				SimulationSpaceToViewportSpace(creatureSnapShot.pos + (-creatureSnapShot.rightDir + creatureSnapShot.forwardDir) * CREATURE_EYE_MAX_PROBE_DISTANCE.value),
				IM_COL32(255, 255, 0, 50),
				UI_CREATURE_TRACKER_DEFAULT_LINE_PIXEL_THICKNESS
			);

			vec2 viewportEyePos = SimulationSpaceToViewportSpace(creatureSnapShot.eyePos);
			float viewportEyeConeRadius = simToViewportScale * creatureSnapShot.eyeConeRadius;

			// Draw eye cone ring
			ImGui::GetBackgroundDrawList()->AddCircle(
				viewportEyePos,
				viewportEyeConeRadius,
				IM_COL32(0, 255, 0, 15),
				int(viewportEyeConeRadius),
				UI_CREATURE_TRACKER_DEFAULT_LINE_PIXEL_THICKNESS
			);

			// Draw eye pupil activation ring
			float pupilActivation = creatureSnapShot.eyePupilSights[0];
			ImGui::GetBackgroundDrawList()->AddCircle(
				viewportEyePos,
				viewportEyeConeRadius * pupilActivation,
				IM_COL32(0, 255, 0, 100),
				int(viewportEyeConeRadius),
				UI_CREATURE_TRACKER_DEFAULT_LINE_PIXEL_THICKNESS
			);

			// Draw eye cones
			float angAdd = 2.0 * M_PI / float(CREATURE_EYE_NUM_OF_CONES);
			float creatureAngle = creatureSnapShot.angle;
			for (unsigned int i = 0; i < CREATURE_EYE_NUM_OF_CONES; ++i)
			{
				float iAng = i * angAdd;

				// Draw cone border lines
				float lineAng = M_PI + iAng - creatureAngle;
				vec2 coneLineDir = vec2(sin(lineAng), cos(lineAng));
				ImGui::GetBackgroundDrawList()->AddLine(
					viewportEyePos,
					viewportEyePos + coneLineDir * viewportEyeConeRadius,
					IM_COL32(0, 255, 0, 15),
					UI_CREATURE_TRACKER_DEFAULT_LINE_PIXEL_THICKNESS
				);

				// Draw cone activation lines
				float activationAng = M_PI + iAng + (angAdd / 2) - creatureAngle;
				vec2 coneActivationDir = vec2(sin(activationAng), cos(activationAng));
				vec2 p1 = viewportEyePos;
				vec2 p2 = viewportEyePos + coneActivationDir * viewportEyeConeRadius;
				float activation = creatureSnapShot.eyeConeSights.data()[i];
				float oneMinusActivation = 1.0 - activation;
				ImGui::GetBackgroundDrawList()->AddLine(
					p1,
					p1 * oneMinusActivation + p2 * activation,
					IM_COL32(0, 255, 0, 100),
					UI_CREATURE_TRACKER_DEFAULT_LINE_PIXEL_THICKNESS
				);
			}
		}
	}

	void UpdateCreatureData()
	{
		creatureSnapShot = GetCreatureSnapshot(creatureID);
	}

	void GatherPlotData()
	{
		plot_Life.push_front(creatureSnapShot.life);
		plot_Energy.push_front(creatureSnapShot.energy);
		plot_Meat.push_front(creatureSnapShot.meat);

		if (plot_Life.size() > UI_CREATURE_TRACKER_PLOT_NUM_OF_VALUES)
		{
			plot_Life.pop_back();
		}
		if (plot_Energy.size() > UI_CREATURE_TRACKER_PLOT_NUM_OF_VALUES)
		{
			plot_Energy.pop_back();
		}
		if (plot_Meat.size() > UI_CREATURE_TRACKER_PLOT_NUM_OF_VALUES)
		{
			plot_Meat.pop_back();
		}
	}

	void Close()
	{
		expiredCreatureTrackers.push_back(creatureID);
		WindowsHandler::UnRegisterWindow(windowID);
	}

public:

	CreatureTracker(CreatureUniqueID creatureID) : creatureID(creatureID)
	{

		cameraFollow = false;

		overlay_Halo = true;
		overlay_ForwardDir = false;
		overlay_RightDir = false;
		overlay_Eye = true;

		// Get initial snapshot
		UpdateCreatureData();

		string title = "Creature ";
		title.append(to_string(creatureID));

		windowID = WindowsHandler::RegisterNewWindow(
			title,
			SimulationSpaceToViewportSpace(creatureSnapShot.pos),
			vec2(UI_CREATURE_TRACKER_MIN_WINDOW_WIDTH, UI_CREATURE_TRACKER_MIN_WINDOW_HEIGHT)
		);
	}

	CreatureTracker(const CreatureTracker&) = delete;

	void Update()
	{

		if (!WindowsHandler::IsWindowActive(windowID))
		{
			Close();
			return;
		}

		// @TODO: Update in intervals instead of every frame...
		try
		{
			// Our creature is valid
			UpdateCreatureData();
			GatherPlotData();
			Show();
		}
		catch (out_of_range& e)
		{
			// If our unique creature ID was out of range, we're no longer valid
			Close();
		}
	}

};


void StartTrackingCreature(CreatureUniqueID creatureID)
{
	// Create new tracker, add to active trackers map
	activeCreatureTrackers.emplace(creatureID, creatureID);
}


void UpdateCreatureTrackers()
{
	// Update all trackers
	for (auto& it : activeCreatureTrackers)
	{
		it.second.Update();
	}

	// Clear closed creature trackers
	for (auto creatureID : expiredCreatureTrackers)
	{
		activeCreatureTrackers.erase(creatureID);
	}

	expiredCreatureTrackers.clear();
}

void CreatureTracking_Init()
{
	creatureTracker_BrainNodePositions.resize(CREATURE_BRAIN_MAX_NUM_OF_NODES);
	creatureTracker_PlotData.resize(UI_CREATURE_TRACKER_PLOT_NUM_OF_VALUES);
}