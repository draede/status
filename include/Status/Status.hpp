/* 
 * Status class
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


#include <Status/StatusCode.hpp>
#include <Status/Detail/StatusMessage.hpp>
#include <cstdint>
#ifdef STATUS_HAS_FMT
	#ifdef _MSC_VER 
		#pragma warning(disable: 6285 26437 26450 26451 26495 26498 26812)
	#endif
	#include <fmt/core.h>
#endif //#ifdef STATUS_HAS_FMT


namespace Status
{

class [[nodiscard]] Status final
{
public:

	Status() : m_data(CodeToData(OK))
	{
	}

	Status(StatusCode code) : m_data(CodeToData(code))
	{
	}

	Status(StatusCode code, const char *message) : m_data(MessageToData(code, message))
	{
	}

#ifdef STATUS_HAS_FMT
	template <typename... T>
	Status(StatusCode code, fmt::format_string<T...> fmt, T &&... args)
	{
		SetMessage(code, fmt, args...);
	}
#endif //#ifdef STATUS_HAS_FMT

	Status(const Status &status)
	{
		if (IsCodeOnly(status.m_data))
		{
			m_data = status.m_data;
		}
		else
		{
			const auto   *status_message = DataToMessage(status.m_data);

			m_data = MessageToData(status_message->m_code, status_message->m_message);
		}
	}

	Status(Status &&status) noexcept : 
		m_data(status.m_data)
	{
		status.m_data = CodeToData(OK);
	}

	Status &operator=(const Status &status)
	{
		FreeMem();
		if (IsCodeOnly(status.m_data))
		{
			m_data = status.m_data;
		}
		else
		{
			const auto   *status_message = DataToMessage(status.m_data);

			m_data = MessageToData(status_message->m_code, status_message->m_message);
		}

		return *this;
	}

	Status &operator=(Status &&status) noexcept
	{
		if (this != &status)
		{
			FreeMem();
			m_data        = status.m_data;
			status.m_data = CodeToData(OK);
		}

		return *this;
	}

	~Status()
	{
		FreeMem();
		m_data = CodeToData(OK);
	}

	bool IsOk() const
	{
		return (OK == GetCode());
	}

	StatusCode GetCode() const
	{
		if (IsCodeOnly(m_data))
		{
			return DataToCode(m_data);
		}
		else
		{
			return DataToMessage(m_data)->m_code;
		}
	}

	const char *GetMessage() const
	{
		if (IsCodeOnly(m_data))
		{
			return "";
		}
		else
		{
			return DataToMessage(m_data)->m_message;
		}
	}

	bool HasMessage() const
	{
		return !IsCodeOnly(m_data);
	}

	void Set(StatusCode code)
	{
		FreeMem();
		m_data = CodeToData(code);
	}

	void Set(StatusCode code, const char *message)
	{
		FreeMem();
		m_data = MessageToData(code, message);
	}

#ifdef STATUS_HAS_FMT
	template <typename... T>
	void Set(StatusCode code, fmt::format_string<T...> fmt, T &&... args)
	{
		FreeMem();
		SetMessage(code, fmt, args...);
	}
#endif //#ifdef STATUS_HAS_FMT

private:

	uintptr_t   m_data;

	static inline bool IsCodeOnly(uintptr_t data)
	{
		return (data & 1) == 0;
	}

	static inline uintptr_t CodeToData(StatusCode code)
	{
		return ((uintptr_t)code) << 1;
	}

	static inline StatusCode DataToCode(uintptr_t data)
	{
		return (StatusCode)(data >> 1);
	}

	static inline uintptr_t MessageToData(Detail::StatusMessage *status_message)
	{
		return ((uintptr_t)(status_message)) + 1;
	}

	static inline uintptr_t MessageToData(StatusCode code, const char *message, size_t message_length = (size_t)-1)
	{
		auto   *status_message = Detail::StatusMessage::Create(code, message, message_length);

		if (nullptr == status_message)
		{
			return CodeToData(code);
		}

		return MessageToData(status_message);
	}

	static inline Detail::StatusMessage *DataToMessage(uintptr_t data)
	{
		return (Detail::StatusMessage *)(data - 1);
	}

	inline void FreeMem()
	{
		if (!IsCodeOnly(m_data))
		{
			DataToMessage(m_data)->Destroy();
		}
	}

#ifdef STATUS_HAS_FMT
	template <typename... T>
	void SetMessage(StatusCode code, fmt::format_string<T...> fmt, T &&... args)
	{
		size_t   message_length  = fmt::formatted_size(fmt, args...);
		auto     *status_message = Detail::StatusMessage::AllocMem(message_length);

		if (nullptr == status_message)
		{
			m_data = CodeToData(code);

			return;
		}

		fmt::format_to_n(status_message->m_message, message_length, fmt, args...);
		status_message->m_code                    = code;
		status_message->m_message[message_length] = 0;
		
		m_data = MessageToData(status_message);
	}
#endif //#ifdef STATUS_HAS_FMT

};

}//namespace Status


#ifdef STATUS_HAS_FMT

template <>
struct fmt::formatter<Status::Status>
{
	static constexpr auto parse(const format_parse_context &ctx)
	{
		return ctx.begin();
	}

	template <typename format_context>
	auto format(const Status::Status &status, format_context &ctx)
	{
		if (status.IsOk())
		{
			return format_to(ctx.out(), "Status(OK)");
		}
		else
		if (!status.HasMessage())
		{
			return format_to(ctx.out(), "Status({})", status.GetCode());
		}
		else
		{
			return format_to(ctx.out(), "Status({}, \"{}\")", status.GetCode(), status.GetMessage());
		}
	}
};

#endif //#ifdef STATUS_HAS_FMT
