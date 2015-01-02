/*
 * Copyright 1999-2004 The Apache Software Foundation.
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
 */
// Class header file...
#include "XalanNullOutputStream.hpp"



XALAN_CPP_NAMESPACE_BEGIN



XalanNullOutputStream::XalanNullOutputStream() :
	XalanOutputStream(1)
{
}



XalanNullOutputStream::~XalanNullOutputStream()
{
}



void
XalanNullOutputStream::writeData(
			const char*		/* theBuffer */,
			size_type		/* theBufferLength */)
{
}



void
XalanNullOutputStream::doFlush()
{
}



XALAN_CPP_NAMESPACE_END