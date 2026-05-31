/* 
 * This file is part of the TeensyPower distribution
 * (https://github.com/LisaRowell/TeensyPower).
 * Copyright (c) 2026 Lisa Rowell
 * 
 * This program is free software: you can redistribute it and/or modify  
 * it under the terms of the GNU General Public License as published by  
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but 
 * WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU 
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License 
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef BMV_CURRENT_FIELD_H
#define BMV_CURRENT_FIELD_H

#include "src/VEDirect/Field.h"

#include <Embedded_Template_Library.h>
#include <etl/vector.h>
#include <etl/string.h>

#include <stdint.h>

class DataModelScaledInt32Leaf;
class MPPTController;

class BMVCurrentField : public Field {
    private:
        DataModelScaledInt32Leaf &dataModelLeaf;
        const etl::ivector<MPPTController *> *mppts;

        void clearMPPTsCurrent();
        void setMPPTsCurrent(int32_t currentMA);

    public:
        BMVCurrentField(const char *deviceName,
                        DataModelScaledInt32Leaf &dataModelLeaf,
                        const etl::ivector<MPPTController *> *mppts = nullptr);
        void set(const etl::istring &message) override;
};

#endif
