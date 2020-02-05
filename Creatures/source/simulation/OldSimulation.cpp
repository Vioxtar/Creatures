#include "Simulation.h"


float simWidth;
float simHeight;

// @TODO: Add simWidth/simHeight changed callback, dont forget to divide by 2!


// Efficient removal of a vector element by swapping it to the last place, and only then popping it (without shifts)
template<typename T>
void SwapRemoveVectorElement(vector<T> vector, unsigned int index)
{
	if (vector.size() > 1)
	{
		iter_swap(vector.begin() + index, vector.end() - 1);
		vector.pop_back();
	}
	else
	{
		vector.clear();
	}
}

void RemoveCreature(unsigned int creature)
{
	if (creature_count <= 0)
		return;

	SwapRemoveVectorElement(creature_colrs, creature);
	SwapRemoveVectorElement(creature_poses, creature);
	SwapRemoveVectorElement(creature_velos, creature);
	SwapRemoveVectorElement(creature_frwds, creature);
	SwapRemoveVectorElement(creature_lives, creature);
	SwapRemoveVectorElement(creature_radii, creature);
	SwapRemoveVectorElement(creature_enrgs, creature);
	SwapRemoveVectorElement(creature_meats, creature);

	creature_count--;
}

unsigned int AddCreature()
{
	creature_colrs.emplace_back();
	creature_poses.emplace_back();
	creature_velos.emplace_back();
	creature_frwds.emplace_back();
	creature_lives.emplace_back();
	creature_radii.emplace_back();
	creature_enrgs.emplace_back();
	creature_meats.emplace_back();

	creature_count++;

	return creature_count - 1;
}

void TickCreatureBodyActuations()
{
	using namespace mathutils;
#pragma omp parallel for
	for (int creature = 0; creature < creature_count; creature++)
	{
		creature_velos.at(creature) += vec2((random() - 0.5) * 2 * 0.0000001, (random() - 0.5) * 2 * 0.0000001);
	}
}

void TickBodyPlacements()
{
#pragma omp parallel for
	for (int creature = 0; creature < creature_count; creature++)
	{
		creature_poses.at(creature) += creature_velos.at(creature);
	}
}

void TickBorderPhysics()
{
	// Right is +, left is -
	// Up is +, down is -

	// Cache some stuff in advance
	const float currentSimWidth = simWidth;
	const float currentSimHeight = simHeight;
	
	const float rightBorder = currentSimWidth;
	const float leftBorder = -currentSimWidth;
	const float upBorder = currentSimHeight;
	const float downBorder = -currentSimHeight;

	// Fast reject thresholds (so we don't have to access radii values we don't need)
	const float maxCreatureSize = SIM_SETTINGS.MAX_CREATURE_RADIUS.value;

	const float rightFastRejectThreshold = rightBorder - maxCreatureSize;
	const float leftFastRejectThreshold = leftBorder + maxCreatureSize;
	const float upFastRejectThreshold = upBorder - maxCreatureSize;
	const float downFastRejectThreshold = downBorder + maxCreatureSize;

#pragma omp parallel for
	for (int creature = 0; creature < creature_count; creature++)
	{
		vec2 pos = creature_poses.at(creature);

		bool closeToRightBorder = pos.x >= rightFastRejectThreshold;
		bool closeToLeftBorder = pos.x <= leftFastRejectThreshold;
		bool closeToUpBorder = pos.y >= upFastRejectThreshold;
		bool closeToDownBorder = pos.y <= downFastRejectThreshold;

		// Fast reject
		if (!closeToRightBorder && !closeToLeftBorder && !closeToUpBorder && !closeToDownBorder)
			continue;

		// We're close to one of the borders, consider creature radius and check more closely
		float violation;
		float rad = creature_radii.at(creature);

		if (closeToRightBorder)
		{
			violation = pos.x + rad - rightBorder;
			if (violation > 0)
			{
				creature_poses.at(creature).x -= violation;
				creature_velos.at(creature).x -= violation;
			}
		}
		if (closeToLeftBorder)
		{
			violation = pos.x - rad - leftBorder;
			if (violation < 0)
			{
				creature_poses.at(creature).x -= violation;
				creature_velos.at(creature).x -= violation;
			}
		}
		if (closeToUpBorder)
		{
			violation = pos.y + rad - upBorder;
			if (violation > 0)
			{
				creature_poses.at(creature).y -= violation;
				creature_velos.at(creature).y -= violation;
			}
		}
		if (closeToDownBorder)
		{
			violation = pos.y - rad - downBorder;
			if (violation < 0)
			{
				creature_poses.at(creature).y -= violation;
				creature_velos.at(creature).y -= violation;
			}
		}
	}
}

void TickBorderPhysicsCompute()
{
	DoCreatureBorderPhysics();
}


void TickCreatureCollisionPhysicsNaive()
{
#pragma omp parallel for schedule(dynamic)
	for (int creatureA = 0; creatureA < creature_count; creatureA++)
	{
		vec2 creatureAPos = creature_poses.at(creatureA);

		for (int creatureB = creatureA + 1; creatureB < creature_count; creatureB++)
		{
			vec2 creatureBPos = creature_poses.at(creatureB);
			vec2 c1c2PosDiff = creatureBPos - creatureAPos;
			float dist = length(c1c2PosDiff);

			if (dist <= 0)
				continue;

			float radiiSum = creature_radii.at(creatureA) + creature_radii.at(creatureB);
			if (dist < radiiSum)
			{
				float violation = radiiSum - dist;
				float push = violation / radiiSum;

				float m1 = 1;
				float m2 = 1;
				float m1Frac = m1 / (m1 + m2);
				float m2Frac = m2 / (m1 + m2);

				vec2 c1PosSub = -c1c2PosDiff;
				vec2 c2PosSub = c1c2PosDiff;

				c1PosSub *= push;
				c2PosSub *= push;

				c1PosSub *= m2Frac;
				c2PosSub *= m1Frac;

				creature_poses.at(creatureA) += c1PosSub;
				creature_poses.at(creatureB) += c2PosSub;

				creature_velos.at(creatureA) += c1PosSub;
				creature_velos.at(creatureB) += c2PosSub;
			}
		}
	}
}

void TickCreatureCollisionPhysicsClever()
{
	DoCreatureCollidePhysics();
}

void InitFirstGenCreature(unsigned int creature)
{
	// @TODO: Actually implement
	using namespace mathutils;
	creature_radii.at(creature) = 0.006;
	creature_colrs.at(creature) = vec3(random(), random(), random());
	creature_lives.at(creature) = random() * 0.5 + 0.3;
}

void InitOffspringCreature(unsigned int offspring, unsigned int parent1, unsigned int parent2)
{

}


void Simulation_Init()
{
	// Init compute utils
	ComputeUtils_Init();

	// Cache simulation dimensions @TODO: Use the callbacks once you've added them! Divide by 2!
	simWidth = SIM_SETTINGS.SIMULATION_WIDTH.value / 2;
	simHeight = SIM_SETTINGS.SIMULATION_HEIGHT.value / 2;

	// Create creatures
	const unsigned int numOfCreaturesOnInit = SIM_SETTINGS.NUM_OF_CREATURES_ON_INIT.value;

	// Initialize creature datas
	{
		creature_colrs.reserve(numOfCreaturesOnInit);
		creature_poses.reserve(numOfCreaturesOnInit);
		creature_velos.reserve(numOfCreaturesOnInit);
		creature_frwds.reserve(numOfCreaturesOnInit);
		creature_lives.reserve(numOfCreaturesOnInit);
		creature_radii.reserve(numOfCreaturesOnInit);
		creature_enrgs.reserve(numOfCreaturesOnInit);
		creature_meats.reserve(numOfCreaturesOnInit);
	}

	// Add first gen creatures
	for (unsigned int i = 1; i <= numOfCreaturesOnInit; i++)
	{
		const unsigned int creature = AddCreature();
		InitFirstGenCreature(creature);
	}
}

void Simulation_Update()
{
	// TickCreatureThinks();
	TickCreatureBodyActuations();
	TickBodyPlacements();
	TickBorderPhysicsCompute();
	TickBorderPhysics();
	//TickCreatureCollisionPhysicsClever();
}
