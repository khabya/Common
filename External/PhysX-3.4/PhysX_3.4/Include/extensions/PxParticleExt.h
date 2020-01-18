//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
//  * Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
//  * Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
//  * Neither the name of NVIDIA CORPORATION nor the names of its
//    contributors may be used to endorse or promote products derived
//    from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ``AS IS'' AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
// PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
// OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Copyright (c) 2008-2018 NVIDIA Corporation. All rights reserved.
// Copyright (c) 2004-2008 AGEIA Technologies, Inc. All rights reserved.
// Copyright (c) 2001-2004 NovodeX AG. All rights reserved.  


#ifndef PX_PHYSICS_EXTENSIONS_PARTICLEEXT_H
#define PX_PHYSICS_EXTENSIONS_PARTICLEEXT_H
/** \addtogroup extensions
  @{
*/

#include "PxPhysXConfig.h"
#include "foundation/PxStrideIterator.h"
#include "foundation/PxBounds3.h"

#if !PX_DOXYGEN
namespace physx
{
#endif

/**
\brief extensions for the partice feature (deprecated)
\deprecated The PhysX particle feature has been deprecated in PhysX version 3.4
*/
class PX_DEPRECATED PxParticleExt
{
public:

	struct ParticleBounds
	{
		PxBounds3 bounds;
		PxU32 firstParticle;
		PxU32 numParticles;		
	};

	/**
	\brief Computes particle bounds by sorting particle positions into a spatial hash grid.

	Given a strided array of particle positions, this function partitions the particles into 
	subsets of nearby particles by mapping the particle positions onto a 3-dimensional grid
	and creating ParticleBounds data structures for each occupied grid cell. 
	
	Each returned ParticleBounds data structure can be used to determine the indices of the particles
	belonging to the same grid cell. Particle indices of one grid cell are returned as contiguous 
	blocks in the sortedParticleIndices buffer and can be found within the range
	[firstParticle, firstParticle + numParticles - 1]. Each returned ParticleBounds further contains
	an AABB "bounds" that tightly fits all particle positions within the grid cell.

	The number of created ParticleBounds can be retrieved via the return value.	The user can cap the 
	maximum amount of generated ParticleBounds via the maxBounds parameter. Particles that would cause
	this maximum to be exceeded will be skipped over and will not be part of any returned ParticleBounds. 
	(Note that this also hold when maxBounds is equal to hashSize. Thus, in the worst case, if no particle 
	should ever be skipped over by the algorithm "hashSize" must be at least equal to numParticles)

	The user is responsible for allocating the output buffers sortedParticleIndices 
	(for at least numParticles entries) and particleBounds (for at least maxBounds entries).

	The size of a cubical grid cell can be tuned by setting the gridSpacing parameter. 
	
	The user supplied hashSize must be a power of two.

	\param[out] sortedParticleIndices Pointer to user allocated array of size numParticles where the sorted particle indices will be written to.
	\param[out] particleBounds Pointer to user allocated array of size maxBounds where the ParticleBounds will be written to.
	\param[in] positionBuffer Strided data of input particle positions.
	\param[in] validParticleRange Range of valid particles within validParticleBitmap. (See PxParticleReadData.validParticleRange).
	\param[in] validParticleBitmap  Bitmap specifying valid slots in positionBuffer. (See PxParticleReadData.validParticleBitmap).
	\param[in] hashSize Hash size used internally by the hashing algorithm. Must be a power of two.
	\param[in] maxBounds Maximum number of bounds to be returned. Must be smaller or equal than hashSize.
	\param[in] gridSpacing Side length of each cubical grid cell.
	\return PxU32. Number of ParticleBounds data structures written to the particleBounds buffer. Smaller or equal than maxBounds.

	*/
	static PxU32 buildBoundsHash(PxU32* sortedParticleIndices,
								 ParticleBounds* particleBounds,
								 const PxStrideIterator<const PxVec3>& positionBuffer,
								 const PxU32 validParticleRange,
								 const PxU32* validParticleBitmap,
								 const PxU32 hashSize,
								 const PxU32 maxBounds,
								 const PxReal gridSpacing);

	/**
	\brief Class to manage particle indices.
	Extension particle index management can be useful if no application side particle index allocation 
	functionality is available.  An PxParticleExt::IndexPool instance is meant to be used for one 
	PxParticleSystem/PxParticleFluid instance.
	The instance can be created with PxParticleExt::createIndexPool().

	@see PxParticleExt::createIndexPool()
	*/
	class IndexPool
	{
	public:
		/**
		\brief Allocates a requested number of indices if possible.
		\param[in] num Number of indices that should be allocated.
		\param[out] indexBuffer Strided data to which allocated indices are written.
		\return PxU32. Number of indices that where allocated by the operation. 
		*/
		virtual		PxU32	allocateIndices(PxU32 num, const PxStrideIterator<PxU32>& indexBuffer)		= 0;
		
		/**
		\brief Deallocates a requested number of indices.
		\param[in] num Number of indices that should be freed.
		\param[in] indexBuffer Strided data describing the indices that need to be freed. It's a requirement 
		to specify unique indices, that where previously allocated by PxParticlesExt::allocateIndices()
		*/
		virtual		void	freeIndices(PxU32 num, const PxStrideIterator<const PxU32>& indexBuffer)	= 0;
		
		/**
		\brief Deallocates all previously allocated indices.
		*/
		virtual		void	freeIndices()																= 0;
		
		/**
		\brief Releases IndexPool instance.
		*/
		virtual		void	release()																	= 0;

		/**
		\brief virtual destructor
		*/
		virtual ~IndexPool() {}
	};

	/**
	\brief Creates a PxParticlesExt::IndexPool instance.
	\param[in] maxParticles Maximum number of available indices.
	*/
	static IndexPool* createIndexPool(PxU32 maxParticles);
	
	/**
	\brief Creates a PxParticlesExt::IndexPool instance initialized with a preallocated set of indices specified with a bitmap.
	\param[in] maxParticles Maximum number of available indices.
	\param[in] validParticleRange Range of indices represented in validParticleBitmap. (See PxParticleReadData.validParticleRange).
	\param[in] validParticleBitmap Bitmap specifying the valid particle indices that should be preallocated. (See PxParticleReadData.validParticleBitmap).
	*/
	static IndexPool* createIndexPool(PxU32 maxParticles, PxU32 validParticleRange, const PxU32* validParticleBitmap);

};

#if !PX_DOXYGEN
} // namespace physx
#endif

/** @} */
#endif
