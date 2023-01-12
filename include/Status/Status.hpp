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

template <typename STATUS_CODE, STATUS_CODE STATUS_CODE_OK>
class [[nodiscard]] Status final
{
public:

	static_assert(sizeof(STATUS_CODE) == sizeof(uint32_t) && std::is_integral<STATUS_CODE>(), 
	              "STATUS_CODE must be a 32 bit integer");

	Status() : m_data(CodeToData(STATUS_CODE_OK))
	{
	}

	Status(STATUS_CODE code) : m_data(CodeToData(code))
	{
	}

	Status(STATUS_CODE code, const char *message) : m_data(MessageToData(code, message))
	{
	}

#ifdef STATUS_HAS_FMT
	template <typename... T>
	Status(STATUS_CODE code, fmt::format_string<T...> fmt, T &&... args)
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
		status.m_data = CodeToData(STATUS_CODE_OK);
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
			status.m_data = CodeToData(STATUS_CODE_OK);
		}

		return *this;
	}

	~Status()
	{
		FreeMem();
		m_data = CodeToData(STATUS_CODE_OK);
	}

	bool IsOk() const
	{
		return (STATUS_CODE_OK == GetCode());
	}

	STATUS_CODE GetCode() const
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

	explicit operator bool() const
	{
		return IsOk();
	}

	explicit operator STATUS_CODE() const
	{
		return GetCode();
	}

	explicit operator const char *() const
	{
		return GetMessage();
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

	void Set(STATUS_CODE code)
	{
		FreeMem();
		m_data = CodeToData(code);
	}

	void Set(STATUS_CODE code, const char *message)
	{
		FreeMem();
		m_data = MessageToData(code, message);
	}

#ifdef STATUS_HAS_FMT
	template <typename... T>
	void Set(STATUS_CODE code, fmt::format_string<T...> fmt, T &&... args)
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

	static inline uintptr_t CodeToData(STATUS_CODE code)
	{
		return ((uintptr_t)code) << 1;
	}

	static inline STATUS_CODE DataToCode(uintptr_t data)
	{
		return (STATUS_CODE)(data >> 1);
	}

	static inline uintptr_t MessageToData(Detail::StatusMessage<STATUS_CODE> *status_message)
	{
		return ((uintptr_t)(status_message)) + 1;
	}

	static inline uintptr_t MessageToData(STATUS_CODE code, const char *message, size_t message_length = (size_t)-1)
	{
		auto   *status_message = Detail::StatusMessage<STATUS_CODE>::Create(code, message, message_length);

		if (nullptr == status_message)
		{
			return CodeToData(code);
		}

		return MessageToData(status_message);
	}

	static inline Detail::StatusMessage<STATUS_CODE> *DataToMessage(uintptr_t data)
	{
		return (Detail::StatusMessage<STATUS_CODE> *)(data - 1);
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
	void SetMessage(STATUS_CODE code, fmt::format_string<T...> fmt, T &&... args)
	{
		size_t   message_length  = fmt::formatted_size(fmt, args...);
		auto     *status_message = Detail::StatusMessage<STATUS_CODE>::AllocMem(message_length);

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

template <typename STATUS_CODE, STATUS_CODE STATUS_CODE_OK>
struct fmt::formatter<Status::Status<STATUS_CODE, STATUS_CODE_OK>>
{
	static constexpr auto parse(const format_parse_context &ctx)
	{
		return ctx.begin();
	}

	template <typename format_context>
	auto format(const Status::Status<STATUS_CODE, STATUS_CODE_OK> &status, format_context &ctx)
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
