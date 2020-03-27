#include "CreatureTracking.h"

class CreatureTracker;

map<CreatureUniqueID, CreatureTracker> activeCreatureTrackers;

// Expired creature trackers to be deleted
vector<CreatureUniqueID> expiredCreatureTrackers;


class CreatureTracker
{
	CreatureUniqueID creatureID;
	CreatureData creatureSnapShot;
	string windowTitle;
	bool active;

	bool cameraFollow;

	bool overlay_Halo;
	bool overlay_ForwardDir;
	bool overlay_RightDir;
	bool overlay_Eye;

	void Show()
	{

		if (!ImGui::Begin(windowTitle.c_str(), &active))
		{
			ImGui::End();
			return;
		}

		ShowMisc();
		ShowEyes();
		ShowBrain();

		ImGui::End();
	}

	void ShowMisc()
	{
		float simToViewportScale = SimulationScaleToViewportScale(1.0);

		// Follow camera
		ImGui::Checkbox("Follow", &cameraFollow);
		if (cameraFollow)
		{
			Camera_InterpolateTo(creatureSnapShot.pos, UI_CREATURE_TRACKER_CAMERA_ZOOM_BASED_FOLLOW_INTERPOLATION_RATE.value);
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
			unsigned int nodeIndex = 0;

			unsigned int numOfLevels = creatureSnapShot.brainStructure[0];
			float spaceBetweenLevels = brainSize.x / (numOfLevels);
			for (unsigned int level = 1; level <= numOfLevels; ++level)
			{
				unsigned int numOfNodesInLevel = creatureSnapShot.brainStructure[level];
				float spaceBetweenNodes = brainSize.y / (numOfNodesInLevel);

				vec2 offset = vec2(spaceBetweenLevels / 2.0, spaceBetweenNodes / 2.0);

				for (unsigned int node = 1; node <= numOfNodesInLevel; ++node)
				{
					vec2 nodeDrawPos = brainPos + offset + nodePos;
					float nodeDrawRadius = 10;
					float nodeActivation = creatureSnapShot.brainNodes[nodeIndex];
					bool negativeNodeActivation = nodeActivation < 0.0;

					// @TEMP @DEBUG
					if (nodeActivation > 1.0 || nodeActivation < -1.0)
					{
						cout << nodeActivation << endl;
					}					

					drawList->AddCircle(
						nodeDrawPos,
						nodeDrawRadius,
						IM_COL32(
							UI_CREATURE_TRACKER_BRAIN_NODE_OUTLINE_COLOR_R,
							UI_CREATURE_TRACKER_BRAIN_NODE_OUTLINE_COLOR_G,
							UI_CREATURE_TRACKER_BRAIN_NODE_OUTLINE_COLOR_B,
							UI_CREATURE_TRACKER_BRAIN_NODE_ALPHA
						),
						UI_CREATURE_TRACKER_BRAIN_NODE_NUM_OF_SEGMENTS,
						UI_CREATURE_TRACKER_BRAIN_NODE_OUTLINE_THICKNESS
					);

					if (negativeNodeActivation)
					{
						drawList->AddCircleFilled(
							nodeDrawPos,
							nodeDrawRadius * -nodeActivation,
							IM_COL32(
								255 - UI_CREATURE_TRACKER_BRAIN_NODE_COLOR_R,
								255 - UI_CREATURE_TRACKER_BRAIN_NODE_COLOR_G,
								255 - UI_CREATURE_TRACKER_BRAIN_NODE_COLOR_B,
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
								UI_CREATURE_TRACKER_BRAIN_NODE_COLOR_R,
								UI_CREATURE_TRACKER_BRAIN_NODE_COLOR_G,
								UI_CREATURE_TRACKER_BRAIN_NODE_COLOR_B,
								UI_CREATURE_TRACKER_BRAIN_NODE_ALPHA
							),
							UI_CREATURE_TRACKER_BRAIN_NODE_NUM_OF_SEGMENTS
						);
					}

					
					nodePos.y += spaceBetweenNodes;
					nodeIndex++;
				}
				nodePos.x += spaceBetweenLevels;
				nodePos.y = 0;
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
			float eyePupilConeCoverageFraction = creatureSnapShot.eyePupilConeCoverageFraction;
			float viewportEyePupilRadius = eyePupilConeCoverageFraction * viewportEyeConeRadius;

			// Draw eye cone ring
			ImGui::GetBackgroundDrawList()->AddCircle(
				viewportEyePos,
				viewportEyeConeRadius,
				IM_COL32(0, 255, 0, 15),
				int(viewportEyeConeRadius),
				UI_CREATURE_TRACKER_DEFAULT_LINE_PIXEL_THICKNESS
			);

			// Draw eye pupil ring
			ImGui::GetBackgroundDrawList()->AddCircle(
				viewportEyePos,
				viewportEyePupilRadius,
				IM_COL32(0, 255, 0, 15),
				int(viewportEyePupilRadius),
				UI_CREATURE_TRACKER_DEFAULT_LINE_PIXEL_THICKNESS
			);

			// Draw eye pupil activation
			float pupilActivation = creatureSnapShot.eyePupilSights[0];
			ImGui::GetBackgroundDrawList()->AddCircle(
				viewportEyePos,
				viewportEyePupilRadius * pupilActivation,
				IM_COL32(0, 255, 0, 100),
				int(viewportEyePupilRadius),
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
					viewportEyePos + coneLineDir * viewportEyePupilRadius,
					viewportEyePos + coneLineDir * viewportEyeConeRadius,
					IM_COL32(0, 255, 0, 15),
					UI_CREATURE_TRACKER_DEFAULT_LINE_PIXEL_THICKNESS
				);

				// Draw cone activation lines
				float activationAng = M_PI + iAng + (angAdd / 2) - creatureAngle;
				vec2 coneActivationDir = vec2(sin(activationAng), cos(activationAng));
				vec2 p1 = viewportEyePos + coneActivationDir * viewportEyePupilRadius;
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

	void Close()
	{
		expiredCreatureTrackers.push_back(creatureID);
	}

public:

	CreatureTracker(CreatureUniqueID creatureID) :
		creatureID(creatureID), active(true)
	{
		windowTitle = "Creature ";
		windowTitle.append(to_string(creatureID));

		cameraFollow = false;

		overlay_Halo = true;
		overlay_ForwardDir = false;
		overlay_RightDir = false;
		overlay_Eye = true;
	}

	CreatureTracker(const CreatureTracker&) = delete;

	void Update()
	{

		if (!active)
		{
			Close();
			return;
		}

		// @TODO: Update in intervals instead of every frame...
		try
		{
			// Our creature is valid
			UpdateCreatureData();
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
	
}