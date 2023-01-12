/* 
 * StatusMessage struct
 *
 * https://github.com/draede/status
 * 
 * Copyright (C) 2023 draede
 *
 * Released under the MIT License.
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */ 

#pragma once


#include <cstdlib>
#include <cstring>
#include <new>


namespace Status::Detail
{

template <typename STATUS_CODE>
struct StatusMessage final
{
	STATUS_CODE   m_code;
	char          m_message[1];

	static inline StatusMessage *AllocMem(size_t message_length)
	{
		return (StatusMessage *)std::malloc(sizeof(StatusMessage) + message_length);
	}

	static inline void FreeMem(StatusMessage *status_message)
	{
		if (nullptr != status_message)
		{
			std::free(status_message);
		}
	}

	static inline StatusMessage *Create(STATUS_CODE code, const char *message, size_t message_length = (size_t)-1)
	{
		if ((size_t)-1 == message_length)
		{
			message_length = std::strlen(message);
		}

		StatusMessage   *status_message = AllocMem(message_length);

		if (nullptr != status_message)
		{
			status_message->m_code = code;
			memcpy(status_message->m_message, message, message_length + 1);
		}

		return status_message;
	}

	inline void Destroy()
	{
		FreeMem(this);
	}
};

}//namespace Status::Detail
