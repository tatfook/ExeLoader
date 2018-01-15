/*++

Copyright (C) 2015 Microsoft Corporation (Original Author)
Copyright (C) 2015 netfabb GmbH

All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice,
this list of conditions and the following disclaimer in the documentation
and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

Abstract:

NMR_ModelWriter_3MF_GCC.h defines 3MF Model Writer Class
for GCC. This model writer exports the in memory represenation into a 3MF file,
using a native ZIP implementation.

--*/

#ifndef __NMR_MODELWRITER_3MF_GCC
#define __NMR_MODELWRITER_3MF_GCC

#include "Model/Writer/NMR_ModelWriter_3MF.h" 

namespace NMR {

	class CModelWriter_3MF_GCC : public CModelWriter_3MF {
	protected:
		// These are OPC dependent functions
		virtual void createPackage(_In_ CModel * pModel);
		virtual void writePackageToStream(_In_ PExportStream pStream);
		virtual void releasePackage();
	public:
		CModelWriter_3MF_GCC() = delete;
		CModelWriter_3MF_GCC(_In_ PModel pModel);
	};

}

#endif // __NMR_MODELWRITER_3MF_GCC
