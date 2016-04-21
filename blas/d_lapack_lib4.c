/**************************************************************************************************
*                                                                                                 *
* This file is part of BLASFEO.                                                                   *
*                                                                                                 *
* BLASFEO -- BLAS For Embedded Optimization.                                                      *
* Copyright (C) 2016 by Gianluca Frison. All rights reserved.                                     *
*                                                                                                 *
* HPMPC is free software; you can redistribute it and/or                                          *
* modify it under the terms of the GNU Lesser General Public                                      *
* License as published by the Free Software Foundation; either                                    *
* version 2.1 of the License, or (at your option) any later version.                              *
*                                                                                                 *
* HPMPC is distributed in the hope that it will be useful,                                        *
* but WITHOUT ANY WARRANTY; without even the implied warranty of                                  *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.                                            *
* See the GNU Lesser General Public License for more details.                                     *
*                                                                                                 *
* You should have received a copy of the GNU Lesser General Public                                *
* License along with HPMPC; if not, write to the Free Software                                    *
* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA                  *
*                                                                                                 *
* Author: Gianluca Frison, giaf (at) dtu.dk                                                       *
*                          gianluca.frison (at) imtek.uni-freiburg.de                             *
*                                                                                                 *
**************************************************************************************************/

#include "../include/d_kernel.h"



void dpotrf_ntnn_l_lib(int m, int n, double *pC, int sdc, double *pD, int sdd, double *inv_diag_D)
	{

	if(m<=0 || n<=0)
		return;

	const int bs = 4;

	int i, j, l;

	i = 0;

#if defined(TARGET_X64_SANDY_BRIDGE) || defined(TARGET_X64_HASWELL)
	for(; i<m-7; i+=8)
		{
		j = 0;
		for(; j<i && j<n-3; j+=4)
			{
			kernel_dtrsm_ntnn_rl_inv_8x4_lib4(j, &pD[i*sdd], sdd, &pD[j*sdd], 1, &pC[j*bs+i*sdc], sdc, &pD[j*bs+i*sdd], sdd, &pD[j*bs+j*sdd], &inv_diag_D[j]);
			}
		if(j<n)
			{
			if(j<i) // dgemm
				{
				kernel_dtrsm_ntnn_rl_inv_8x4_vs_lib4(j, &pD[i*sdd], sdd, &pD[j*sdd], 1, &pC[j*bs+i*sdc], sdc, &pD[j*bs+i*sdd], sdd, &pD[j*bs+j*sdd], &inv_diag_D[j], 8, n-j);
				}
			else // dsyrk
				{
				kernel_dpotrf_ntnn_l_8x4_vs_lib4(j, &pD[i*sdd], sdd, &pD[j*sdd], 1, &pC[j*bs+j*sdc], sdc, &pD[j*bs+j*sdd], sdd, &inv_diag_D[j], 8, n-j);
				if(j<n-4)
					{
					kernel_dpotrf_ntnn_l_4x4_vs_lib4(j+4, &pD[(i+4)*sdd], &pD[(j+4)*sdd], 1, &pC[(j+4)*bs+(i+4)*sdc], &pD[(j+4)*bs+(i+4)*sdd], &inv_diag_D[j+4], 4, n-j-4);
					}
				}
			}
		}
	if(m>i)
		{
		if(m-i<=4)
			{
			goto left_4;
			}
		else
			{
			goto left_8;
			}
		}
#else
	for(; i<m-3; i+=4)
		{
		j = 0;
		for(; j<i && j<n-3; j+=4)
			{
			kernel_dtrsm_ntnn_rl_inv_4x4_lib4(j, &pD[i*sdd], &pD[j*sdd], 1, &pC[j*bs+i*sdc], &pD[j*bs+i*sdd], &pD[j*bs+j*sdd], &inv_diag_D[j]);
			}
		if(j<n)
			{
			if(i<j) // dgemm
				{
				kernel_dtrsm_ntnn_rl_inv_4x4_vs_lib4(j, &pD[i*sdd], &pD[j*sdd], 1, &pC[j*bs+i*sdc], &pD[j*bs+i*sdd], &pD[j*bs+j*sdd], &inv_diag_D[j], 4, n-j);
				}
			else // dsyrk
				{
				kernel_dpotrf_ntnn_l_4x4_vs_lib4(j, &pD[i*sdd], &pD[j*sdd], 1, &pC[j*bs+j*sdc], &pD[j*bs+j*sdd], &inv_diag_D[j], 4, n-j);
				}
			}
		}
	if(m>i)
		{
		goto left_4;
		}
#endif

	// common return if i==m
	return;

	// clean up loops definitions

#if defined(TARGET_X64_SANDY_BRIDGE) || defined(TARGET_X64_HASWELL)
	left_8:
	j = 0;
	for(; j<i && j<n-3; j+=4)
		{
		kernel_dtrsm_ntnn_rl_inv_8x4_vs_lib4(j, &pD[i*sdd], sdd, &pD[j*sdd], 1, &pC[j*bs+i*sdc], sdc, &pD[j*bs+i*sdd], sdd, &pD[j*bs+j*sdd], &inv_diag_D[j], m-i, n-j);
		}
	if(j<n)
		{
		if(j<i) // dgemm
			{
			kernel_dtrsm_ntnn_rl_inv_8x4_vs_lib4(j, &pD[i*sdd], sdd, &pD[j*sdd], 1, &pC[j*bs+i*sdc], sdc, &pD[j*bs+i*sdd], sdd, &pD[j*bs+j*sdd], &inv_diag_D[j], m-i, n-j);
			}
		else // dsyrk
			{
			kernel_dpotrf_ntnn_l_8x4_vs_lib4(j, &pD[i*sdd], sdd, &pD[j*sdd], 1, &pC[j*bs+j*sdc], sdc, &pD[j*bs+j*sdd], sdd, &inv_diag_D[j], m-i, n-j);
			if(j<n-4)
				{
				kernel_dpotrf_ntnn_l_4x4_vs_lib4(j+4, &pD[(i+4)*sdd], &pD[(j+4)*sdd], 1, &pC[(j+4)*bs+(i+4)*sdc], &pD[(j+4)*bs+(i+4)*sdd], &inv_diag_D[j+4], m-i-4, n-j-4);
				}
			}
		}
	return;
#endif

	left_4:
	j = 0;
	for(; j<i && j<n-3; j+=4)
		{
		kernel_dtrsm_ntnn_rl_inv_4x4_vs_lib4(j, &pD[i*sdd], &pD[j*sdd], 1, &pC[j*bs+i*sdc], &pD[j*bs+i*sdd], &pD[j*bs+j*sdd], &inv_diag_D[j], m-i, n-j);
		}
	if(j<n)
		{
		if(j<i) // dgemm
			{
			kernel_dtrsm_ntnn_rl_inv_4x4_vs_lib4(j, &pD[i*sdd], &pD[j*sdd], 1, &pC[j*bs+i*sdc], &pD[j*bs+i*sdd], &pD[j*bs+j*sdd], &inv_diag_D[j], m-i, n-j);
			}
		else // dsyrk
			{
			kernel_dpotrf_ntnn_l_4x4_vs_lib4(j, &pD[i*sdd], &pD[j*sdd], 1, &pC[j*bs+j*sdc], &pD[j*bs+j*sdd], &inv_diag_D[j], m-i, n-j);
			}
		}
	return;

	}



void dsyrk_dpotrf_ntnn_l_lib(int m, int n, int k, double *pA, int sda, double *pB, int sdb, int alg, double *pC, int sdc, double *pD, int sdd, double *inv_diag_D)
	{

	if(m<=0 || n<=0)
		return;

	const int bs = 4;

	int i, j, l;

	i = 0;

#if defined(TARGET_X64_SANDY_BRIDGE) || defined(TARGET_X64_HASWELL)
	for(; i<m-7; i+=8)
		{
		j = 0;
		for(; j<i && j<n-3; j+=4)
			{
			kernel_dgemm_dtrsm_ntnn_rl_inv_8x4_lib4(k, &pA[i*sda], sda, &pB[j*sdb], j, &pD[i*sdd], sdd, &pD[j*sdd], alg, &pC[j*bs+i*sdc], sdc, &pD[j*bs+i*sdd], sdd, &pD[j*bs+j*sdd], &inv_diag_D[j]);
			}
		if(j<n)
			{
			if(j<i) // dgemm
				{
				kernel_dgemm_dtrsm_ntnn_rl_inv_8x4_vs_lib4(k, &pA[i*sda], sda, &pB[j*sdb], j, &pD[i*sdd], sdd, &pD[j*sdd], alg, &pC[j*bs+i*sdc], sdc, &pD[j*bs+i*sdd], sdd, &pD[j*bs+j*sdd], &inv_diag_D[j], 8, n-j);
				}
			else // dsyrk
				{
				kernel_dsyrk_dpotrf_ntnn_l_8x4_vs_lib4(k, &pA[i*sda], sda, &pB[j*sdb], j, &pD[i*sdd], sdd, &pD[j*sdd], alg, &pC[j*bs+j*sdc], sdc, &pD[j*bs+j*sdd], sdd, &inv_diag_D[j], 8, n-j);
				if(j<n-4)
					{
					kernel_dsyrk_dpotrf_ntnn_l_4x4_vs_lib4(k, &pA[(i+4)*sda], &pB[(j+4)*sdb], j+4, &pD[(i+4)*sdd], &pD[(j+4)*sdd], alg, &pC[(j+4)*bs+(i+4)*sdc], &pD[(j+4)*bs+(i+4)*sdd], &inv_diag_D[j+4], 4, n-j-4);
					}
				}
			}
		}
	if(m>i)
		{
		if(m-i<=4)
			{
			goto left_4;
			}
		else
			{
			goto left_8;
			}
		}
#else
	for(; i<m-3; i+=4)
		{
		j = 0;
		for(; j<i && j<n-3; j+=4)
			{
			kernel_dgemm_dtrsm_ntnn_rl_inv_4x4_lib4(k, &pA[i*sda], &pB[j*sdb], j, &pD[i*sdd], &pD[j*sdd], alg, &pC[j*bs+i*sdc], &pD[j*bs+i*sdd], &pD[j*bs+j*sdd], &inv_diag_D[j]);
			}
		if(j<n)
			{
			if(i<j) // dgemm
				{
				kernel_dgemm_dtrsm_ntnn_rl_inv_4x4_vs_lib4(k, &pA[i*sda], &pB[j*sdb], j, &pD[i*sdd], &pD[j*sdd], alg, &pC[j*bs+i*sdc], &pD[j*bs+i*sdd], &pD[j*bs+j*sdd], &inv_diag_D[j], 4, n-j);
				}
			else // dsyrk
				{
				kernel_dsyrk_dpotrf_ntnn_l_4x4_vs_lib4(k, &pA[i*sda], &pB[j*sdb], j, &pD[i*sdd], &pD[j*sdd], alg, &pC[j*bs+j*sdc], &pD[j*bs+j*sdd], &inv_diag_D[j], 4, n-j);
				}
			}
		}
	if(m>i)
		{
		goto left_4;
		}
#endif

	// common return if i==m
	return;

	// clean up loops definitions

#if defined(TARGET_X64_SANDY_BRIDGE) || defined(TARGET_X64_HASWELL)
	left_8:
	j = 0;
	for(; j<i && j<n-3; j+=4)
		{
		kernel_dgemm_dtrsm_ntnn_rl_inv_8x4_vs_lib4(k, &pA[i*sda], sda, &pB[j*sdb], j, &pD[i*sdd], sdd, &pD[j*sdd], alg, &pC[j*bs+i*sdc], sdc, &pD[j*bs+i*sdd], sdd, &pD[j*bs+j*sdd], &inv_diag_D[j], m-i, n-j);
		}
	if(j<n)
		{
		if(j<i) // dgemm
			{
			kernel_dgemm_dtrsm_ntnn_rl_inv_8x4_vs_lib4(k, &pA[i*sda], sda, &pB[j*sdb], j, &pD[i*sdd], sdd, &pD[j*sdd], alg, &pC[j*bs+i*sdc], sdc, &pD[j*bs+i*sdd], sdd, &pD[j*bs+j*sdd], &inv_diag_D[j], m-i, n-j);
			}
		else // dsyrk
			{
			kernel_dsyrk_dpotrf_ntnn_l_8x4_vs_lib4(k, &pA[i*sda], sda, &pB[j*sdb], j, &pD[i*sdd], sdd, &pD[j*sdd], alg, &pC[j*bs+j*sdc], sdc, &pD[j*bs+j*sdd], sdd, &inv_diag_D[j], m-i, n-j);
			if(j<n-4)
				{
				kernel_dsyrk_dpotrf_ntnn_l_4x4_vs_lib4(k, &pA[(i+4)*sda], &pB[(j+4)*sdb], j+4, &pD[(i+4)*sdd], &pD[(j+4)*sdd], alg, &pC[(j+4)*bs+(i+4)*sdc], &pD[(j+4)*bs+(i+4)*sdd], &inv_diag_D[j+4], m-i-4, n-j-4);
				}
			}
		}
	return;
#endif

	left_4:
	j = 0;
	for(; j<i && j<n-3; j+=4)
		{
		kernel_dgemm_dtrsm_ntnn_rl_inv_4x4_vs_lib4(k, &pA[i*sda], &pB[j*sdb], j, &pD[i*sdd], &pD[j*sdd], alg, &pC[j*bs+i*sdc], &pD[j*bs+i*sdd], &pD[j*bs+j*sdd], &inv_diag_D[j], m-i, n-j);
		}
	if(j<n)
		{
		if(j<i) // dgemm
			{
			kernel_dgemm_dtrsm_ntnn_rl_inv_4x4_vs_lib4(k, &pA[i*sda], &pB[j*sdb], j, &pD[i*sdd], &pD[j*sdd], alg, &pC[j*bs+i*sdc], &pD[j*bs+i*sdd], &pD[j*bs+j*sdd], &inv_diag_D[j], m-i, n-j);
			}
		else // dsyrk
			{
			kernel_dsyrk_dpotrf_ntnn_l_4x4_vs_lib4(k, &pA[i*sda], &pB[j*sdb], j, &pD[i*sdd], &pD[j*sdd], alg, &pC[j*bs+j*sdc], &pD[j*bs+j*sdd], &inv_diag_D[j], m-i, n-j);
			}
		}
	return;

	}



void dlauum_dpotrf_blk_ntnn_l_lib(int m, int n, int nv, int *rv, int *cv, double *pA, int sda, double *pB, int sdb, int alg, double *pC, int sdc, double *pD, int sdd, double *inv_diag_D)
	{

	if(m<=0 || n<=0)
		return;
	
	// TODO remove
	int k = cv[nv-1];

	const int bs = 4;

	int i, j, l;
	int ii, iii, jj, kii, kiii, kjj, k0, k1;

	i = 0;
	ii = 0;
	iii = 0;

#if defined(TARGET_X64_SANDY_BRIDGE) || defined(TARGET_X64_HASWELL)
	for(; i<m-7; i+=8)
		{

		while(ii<nv && rv[ii]<i+8)
			ii++;
		if(ii<nv)
			kii = cv[ii];
		else
			kii = cv[ii-1];

		j = 0;
		jj = 0;
		for(; j<i && j<n-3; j+=4)
			{

			while(jj<nv && rv[jj]<j+4)
				jj++;
			if(jj<nv)
				kjj = cv[jj];
			else
				kjj = cv[jj-1];
			k0 = kii<kjj ? kii : kjj;

			kernel_dgemm_dtrsm_ntnn_rl_inv_8x4_lib4(k0, &pA[i*sda], sda, &pB[j*sdb], j, &pD[i*sdd], sdd, &pD[j*sdd], alg, &pC[j*bs+i*sdc], sdc, &pD[j*bs+i*sdd], sdd, &pD[j*bs+j*sdd], &inv_diag_D[j]);
			}
		if(j<n)
			{

			while(jj<nv && rv[jj]<j+4)
				jj++;
			if(jj<nv)
				kjj = cv[jj];
			else
				kjj = cv[jj-1];
			k0 = kii<kjj ? kii : kjj;

			if(j<i) // dgemm
				{
				kernel_dgemm_dtrsm_ntnn_rl_inv_8x4_vs_lib4(k0, &pA[i*sda], sda, &pB[j*sdb], j, &pD[i*sdd], sdd, &pD[j*sdd], alg, &pC[j*bs+i*sdc], sdc, &pD[j*bs+i*sdd], sdd, &pD[j*bs+j*sdd], &inv_diag_D[j], 8, n-j);
				}
			else // dsyrk
				{
				kernel_dsyrk_dpotrf_ntnn_l_8x4_vs_lib4(k0, &pA[i*sda], sda, &pB[j*sdb], j, &pD[i*sdd], sdd, &pD[j*sdd], alg, &pC[j*bs+j*sdc], sdc, &pD[j*bs+j*sdd], sdd, &inv_diag_D[j], 8, n-j);
				if(j<n-4)
					{
					kernel_dsyrk_dpotrf_ntnn_l_4x4_vs_lib4(k, &pA[(i+4)*sda], &pB[(j+4)*sdb], j+4, &pD[(i+4)*sdd], &pD[(j+4)*sdd], alg, &pC[(j+4)*bs+(j+4)*sdc], &pD[(j+4)*bs+(j+4)*sdd], &inv_diag_D[j+4], 4, n-j-4); // TODO
					}
				}
			}
		}
	if(m>i)
		{
		if(m-i<=4)
			{
			goto left_4;
			}
		else
			{
			goto left_8;
			}
		}
#else
	for(; i<m-3; i+=4)
		{

		while(ii<nv && rv[ii]<i+4)
			ii++;
		if(ii<nv)
			kii = cv[ii];
		else
			kii = cv[ii-1];

		j = 0;
		jj = 0;
		for(; j<i && j<n-3; j+=4)
			{

			while(jj<nv && rv[jj]<j+4)
				jj++;
			if(jj<nv)
				kjj = cv[jj];
			else
				kjj = cv[jj-1];
			k0 = kii<kjj ? kii : kjj;

			kernel_dgemm_dtrsm_ntnn_rl_inv_4x4_lib4(k0, &pA[i*sda], &pB[j*sdb], j, &pD[i*sdd], &pD[j*sdd], alg, &pC[j*bs+i*sdc], &pD[j*bs+i*sdd], &pD[j*bs+j*sdd], &inv_diag_D[j]);
			}
		if(j<n)
			{

			while(jj<nv && rv[jj]<j+4)
				jj++;
			if(jj<nv)
				kjj = cv[jj];
			else
				kjj = cv[jj-1];
			k0 = kii<kjj ? kii : kjj;

			if(i<j) // dgemm
				{
				kernel_dgemm_dtrsm_ntnn_rl_inv_4x4_vs_lib4(k0, &pA[i*sda], &pB[j*sdb], j, &pD[i*sdd], &pD[j*sdd], alg, &pC[j*bs+i*sdc], &pD[j*bs+i*sdd], &pD[j*bs+j*sdd], &inv_diag_D[j], 4, n-j);
				}
			else // dsyrk
				{
				kernel_dsyrk_dpotrf_ntnn_l_4x4_vs_lib4(k0, &pA[i*sda], &pB[j*sdb], j, &pD[i*sdd], &pD[j*sdd], alg, &pC[j*bs+j*sdc], &pD[j*bs+j*sdd], &inv_diag_D[j], 4, n-j);
				}
			}
		}
	if(m>i)
		{
		goto left_4;
		}
#endif

	// common return if i==m
	return;

	// clean up loops definitions

#if defined(TARGET_X64_SANDY_BRIDGE) || defined(TARGET_X64_HASWELL)
	left_8:

	kii = cv[nv-1];

	j = 0;
	jj = 0;
	for(; j<i && j<n-3; j+=4)
		{

		while(jj<nv && rv[jj]<j+4)
			jj++;
		if(jj<nv)
			kjj = cv[jj];
		else
			kjj = cv[jj-1];
		k0 = kii<kjj ? kii : kjj;

		kernel_dgemm_dtrsm_ntnn_rl_inv_8x4_vs_lib4(k0, &pA[i*sda], sda, &pB[j*sdb], j, &pD[i*sdd], sdd, &pD[j*sdd], alg, &pC[j*bs+i*sdc], sdc, &pD[j*bs+i*sdd], sdd, &pD[j*bs+j*sdd], &inv_diag_D[j], m-i, n-j);
		}
	if(j<n)
		{

		while(jj<nv && rv[jj]<j+4)
			jj++;
		if(jj<nv)
			kjj = cv[jj];
		else
			kjj = cv[jj-1];
		k0 = kii<kjj ? kii : kjj;

		if(j<i) // dgemm
			{
			kernel_dgemm_dtrsm_ntnn_rl_inv_8x4_vs_lib4(k0, &pA[i*sda], sda, &pB[j*sdb], j, &pD[i*sdd], sdd, &pD[j*sdd], alg, &pC[j*bs+i*sdc], sdc, &pD[j*bs+i*sdd], sdd, &pD[j*bs+j*sdd], &inv_diag_D[j], m-i, n-j);
			}
		else // dsyrk
			{
			kernel_dsyrk_dpotrf_ntnn_l_8x4_vs_lib4(k0, &pA[i*sda], sda, &pB[j*sdb], j, &pD[i*sdd], sdd, &pD[j*sdd], alg, &pC[j*bs+j*sdc], sdc, &pD[j*bs+j*sdd], sdd, &inv_diag_D[j], m-i, n-j);
			if(j<n-4)
				{
				kernel_dsyrk_dpotrf_ntnn_l_4x4_vs_lib4(k, &pA[(i+4)*sda], &pB[(j+4)*sdb], j+4, &pD[(i+4)*sdd], &pD[(j+4)*sdd], alg, &pC[(j+4)*bs+(j+4)*sdc], &pD[(j+4)*bs+(j+4)*sdd], &inv_diag_D[j+4], m-i-4, n-j-4); // TODO
				}
			}
		}
	return;
#endif

	left_4:

	kii = cv[nv-1];

	j = 0;
	jj = 0;
	for(; j<i && j<n-3; j+=4)
		{

		while(jj<nv && rv[jj]<j+4)
			jj++;
		if(jj<nv)
			kjj = cv[jj];
		else
			kjj = cv[jj-1];
		k0 = kii<kjj ? kii : kjj;

		kernel_dgemm_dtrsm_ntnn_rl_inv_4x4_vs_lib4(k0, &pA[i*sda], &pB[j*sdb], j, &pD[i*sdd], &pD[j*sdd], alg, &pC[j*bs+i*sdc], &pD[j*bs+i*sdd], &pD[j*bs+j*sdd], &inv_diag_D[j], m-i, n-j);
		}
	if(j<n)
		{

		while(jj<nv && rv[jj]<j+4)
			jj++;
		if(jj<nv)
			kjj = cv[jj];
		else
			kjj = cv[jj-1];
		k0 = kii<kjj ? kii : kjj;

		if(j<i) // dgemm
			{
			kernel_dgemm_dtrsm_ntnn_rl_inv_4x4_vs_lib4(k0, &pA[i*sda], &pB[j*sdb], j, &pD[i*sdd], &pD[j*sdd], alg, &pC[j*bs+i*sdc], &pD[j*bs+i*sdd], &pD[j*bs+j*sdd], &inv_diag_D[j], m-i, n-j);
			}
		else // dsyrk
			{
			kernel_dsyrk_dpotrf_ntnn_l_4x4_vs_lib4(k0, &pA[i*sda], &pB[j*sdb], j, &pD[i*sdd], &pD[j*sdd], alg, &pC[j*bs+j*sdc], &pD[j*bs+j*sdd], &inv_diag_D[j], m-i, n-j);
			}
		}
	return;

	}



