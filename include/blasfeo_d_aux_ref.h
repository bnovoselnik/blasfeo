/**************************************************************************************************
*                                                                                                 *
* This file is part of BLASFEO.                                                                   *
*                                                                                                 *
* BLASFEO -- BLAS For Embedded Optimization.                                                      *
* Copyright (C) 2016-2018 by Gianluca Frison.                                                     *
* Developed at IMTEK (University of Freiburg) under the supervision of Moritz Diehl.              *
* All rights reserved.                                                                            *
*                                                                                                 *
* This program is free software: you can redistribute it and/or modify                            *
* it under the terms of the GNU General Public License as published by                            *
* the Free Software Foundation, either version 3 of the License, or                               *
* (at your option) any later version                                                              *.
*                                                                                                 *
* This program is distributed in the hope that it will be useful,                                 *
* but WITHOUT ANY WARRANTY; without even the implied warranty of                                  *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                                   *
* GNU General Public License for more details.                                                    *
*                                                                                                 *
* You should have received a copy of the GNU General Public License                               *
* along with this program.  If not, see <https://www.gnu.org/licenses/>.                          *
*                                                                                                 *
* The authors designate this particular file as subject to the "Classpath" exception              *
* as provided by the authors in the LICENSE file that accompained this code.                      *
*                                                                                                 *
* Author: Gianluca Frison, gianluca.frison (at) imtek.uni-freiburg.de                             *
*                                                                                                 *
**************************************************************************************************/

/*
 * auxiliary algebra operations header
 *
 * include/blasfeo_aux_lib*.h
 *
 */

#ifndef BLASFEO_D_AUX_REF_H_
#define BLASFEO_D_AUX_REF_H_

#include "blasfeo_common.h"

#ifdef __cplusplus
extern "C" {
#endif


// expose reference BLASFEO for testing

void blasfeo_create_dmat_ref  (int m, int n, struct blasfeo_dmat_ref *sA, void *memory);
void blasfeo_create_dvec_ref  (int m, int n, struct blasfeo_dvec_ref *sA, void *memory);

void blasfeo_free_dmat_ref(struct blasfeo_dmat_ref *sA);
void blasfeo_free_dvec_ref(struct blasfeo_dvec_ref *sa);

void blasfeo_pack_dmat_ref    (int m, int n, double *A, int lda, struct blasfeo_dmat_ref *sA, int ai, int aj);
int  blasfeo_memsize_dmat_ref (int m, int n);

void blasfeo_dgead_ref(int m, int n,
					double alpha,\
					struct blasfeo_dmat_ref *sA, int ai, int aj,\
					struct blasfeo_dmat_ref *sB, int bi, int bj);

void blasfeo_dgecp_ref(int m, int n,\
					struct blasfeo_dmat_ref *sA, int ai, int aj,\
					struct blasfeo_dmat_ref *sB, int bi, int bj);
void blasfeo_dgesc_ref(int m, int n,\
					double alpha,\
					struct blasfeo_dmat_ref *sA, int ai, int aj);
void blasfeo_dgecpsc_ref(int m, int n,
					double alpha,\
					struct blasfeo_dmat_ref *sA, int ai, int aj,\
					struct blasfeo_dmat_ref *sB, int bi, int bj);


#ifdef __cplusplus
}
#endif

#endif  // BLASFEO_D_AUX_REF_H_
