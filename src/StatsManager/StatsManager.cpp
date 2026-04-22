/*
 * This file is part of the TeensyPower distribution
 * (https://github.com/LisaRowell/TeensyPower).
 * Copyright (C) 2021-2024, 2026 Lisa Rowell
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "StatsManager.h"
#include "StatsHolder.h"

#include "../Util/PassiveTimer.h"
#include "../Util/Logger.h"

#include <Embedded_Template_Library.h>
#include <etl/vector.h>
#include <etl/iterator.h>

#include <stdint.h>

StatsManager::StatsManager()
    : harvesting(false),
      statsHolders() {
    harvestTimer.setMilliSeconds(statsUpdateTimeIntervalMs);
}

void StatsManager::addStatsHolder(StatsHolder &statsHolder) {
    statsHolders.push_back(statsHolder);
}

void StatsManager::service() {
    if (!harvesting && harvestTimer.expired()) {
        harvesting = true;
        harvestTimer.advanceMilliSeconds(statsUpdateTimeIntervalMs);
        statHolderIterator = statsHolders.begin();
    }
    if (harvesting) {
        if (statHolderIterator != statsHolders.end()) {
            StatsHolder &statHolder = *statHolderIterator;
            statHolder.exportStats();
            statHolderIterator++;
        } else {
            harvesting = false;
        }
    }
}
