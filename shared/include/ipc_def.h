/******************************************************************************
* File Name:   ipc_def.h
*
* Description:
*  Auxiliary header with constants and structures for the IPC.
*
*******************************************************************************
* Copyright 2022 Cypress Semiconductor Corporation (an Infineon company) or
* an affiliate of Cypress Semiconductor Corporation.
* SPDX-License-Identifier: Apache-2.0
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*******************************************************************************/

#ifndef IPC_DEF_H
#define IPC_DEF_H

    /* Enable/Disable the usage of IPC Semaphore */
    #define ENABLE_SEMA             1u

    /* Semaphore number to be used in this example. Semaphores 0-15 are reserved
       for system use. */
    #define SEMA_NUM                16u

#endif /* IPC_DEF_H */

/* [] END OF FILE */
