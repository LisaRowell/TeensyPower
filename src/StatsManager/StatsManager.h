#include <iterator>
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

#ifndef STATS_MANAGER_H
#define STATS_MANAGER_H

#include "StatsHolder.h"

#include "../Util/PassiveTimer.h"

#include <Embedded_Template_Library.h>
#include <etl/intrusive_list.h>

#include <stddef.h>
#include <stdint.h>

class StatsHolder;

class StatsManager {
    private:
        static constexpr uint32_t statsUpdateTimeIntervalMs = 10 * 1000;

        bool harvesting;
        PassiveTimer harvestTimer;
        etl::intrusive_list<StatsHolder, statsHolderLink> statsHolders;
        etl::intrusive_list<StatsHolder, statsHolderLink>::iterator statHolderIterator;

    public:
        StatsManager();
        void addStatsHolder(StatsHolder &statsHolder);
        void service();
};

#endif
