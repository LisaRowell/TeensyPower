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

#ifndef STATS_HOLDER_H
#define STATS_HOLDER_H

#include <Embedded_Template_Library.h>
#include <etl/intrusive_links.h>

#include <stdint.h>

constexpr size_t statsHolderLinkID = 0;
typedef etl::bidirectional_link<statsHolderLinkID> statsHolderLink;

class StatsHolder : public statsHolderLink {
    private:
        uint32_t lastHarvestTime;

    protected:
        virtual void harvestStats(uint32_t msElapsed) = 0;

    public:
        StatsHolder();
        void exportStats();
};

#endif // STATS_HOLDER_H
