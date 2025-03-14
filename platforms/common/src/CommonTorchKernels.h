#ifndef COMMON_TORCH_KERNELS_H_
#define COMMON_TORCH_KERNELS_H_

/* -------------------------------------------------------------------------- *
 *                                   OpenMM                                   *
 * -------------------------------------------------------------------------- *
 * This is part of the OpenMM molecular simulation toolkit originating from   *
 * Simbios, the NIH National Center for Physics-Based Simulation of           *
 * Biological Structures at Stanford, funded under the NIH Roadmap for        *
 * Medical Research, grant U54 GM072970. See https://simtk.org.               *
 *                                                                            *
 * Portions copyright (c) 2018-2024 Stanford University and the Authors.      *
 * Authors: Peter Eastman                                                     *
 * Contributors:                                                              *
 *                                                                            *
 * Permission is hereby granted, free of charge, to any person obtaining a    *
 * copy of this software and associated documentation files (the "Software"), *
 * to deal in the Software without restriction, including without limitation  *
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,   *
 * and/or sell copies of the Software, and to permit persons to whom the      *
 * Software is furnished to do so, subject to the following conditions:       *
 *                                                                            *
 * The above copyright notice and this permission notice shall be included in *
 * all copies or substantial portions of the Software.                        *
 *                                                                            *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR *
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,   *
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL    *
 * THE AUTHORS, CONTRIBUTORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,    *
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR      *
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE  *
 * USE OR OTHER DEALINGS IN THE SOFTWARE.                                     *
 * -------------------------------------------------------------------------- */

#include "TorchKernels.h"
#include "openmm/common/ComputeContext.h"
#include "openmm/common/ComputeArray.h"
#include <set>

namespace TorchPlugin {

/**
 * This kernel is invoked by TorchForce to calculate the forces acting on the system and the energy of the system.
 */
class CommonCalcTorchForceKernel : public CalcTorchForceKernel {
public:
    CommonCalcTorchForceKernel(std::string name, const OpenMM::Platform& platform, OpenMM::ComputeContext& cc) :
            CalcTorchForceKernel(name, platform), hasInitializedKernel(false), cc(cc) {
    }
    ~CommonCalcTorchForceKernel();
    /**
     * Initialize the kernel.
     * 
     * @param system         the System this kernel will be applied to
     * @param force          the TorchForce this kernel will be used for
     * @param module         the PyTorch module to use for computing forces and energy
     */
    void initialize(const OpenMM::System& system, const TorchForce& force, torch::jit::script::Module& module);
    /**
     * Execute the kernel to calculate the forces and/or energy.
     *
     * @param context        the context in which to execute this kernel
     * @param includeForces  true if forces should be calculated
     * @param includeEnergy  true if the energy should be calculated
     * @return the potential energy due to the force
     */
    double execute(OpenMM::ContextImpl& context, bool includeForces, bool includeEnergy);
private:
    bool hasInitializedKernel;
    OpenMM::ComputeContext& cc;
    torch::jit::script::Module module;
    std::vector<std::string> globalNames;
    std::set<std::string> paramDerivs;
    bool usePeriodic, outputsForces;
    OpenMM::ComputeArray networkForces;
    OpenMM::ComputeKernel addForcesKernel;
};

} // namespace TorchPlugin

#endif /*COMMON_TORCH_KERNELS_H_*/
