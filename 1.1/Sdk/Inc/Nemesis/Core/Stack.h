//======================================================================================
// Copyright(C) Piranha Bytes GmbH & THQ Nordic GmbH 2023
//======================================================================================
// This file is part of the Nemesis Profiler.
// Nemesis Profiler is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
// Nemesis Profiler is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
// You should have received a copy of the GNU General Public License along with Nemesis Profiler. If not, see <https://www.gnu.org/licenses/>.
//======================================================================================
#pragma once

//======================================================================================
#include "Array.h"

//======================================================================================
namespace nemesis
{
	typedef class Stack_s *Stack_t;

	struct Page_s
	{
		uint8_t* Data;
		uint8_t* End;
		uint8_t* Pos;

		size_t Used()     const { return Pos-Data; }
		size_t Remain()   const { return End-Pos;  }
		size_t Capacity() const { return End-Data; }

		void Reset() { Pos = Data; }
	};

	class Stack_s
	{
		enum 
		{ MIN_PAGE_SIZE		= 1024
		, DEFAULT_PAGE_SIZE	= 4096
		, DEFAULT_ALIGN		= 8
		};

	public:
		explicit Stack_s( Allocator_t alloc );
		Stack_s();
		~Stack_s();

	public:
		void Init( Allocator_t alloc, size_t page_size );
		void Init( Allocator_t alloc );
		void Clear();
		void Reset();

	public:
		uint64_t GetPos() const;
		void	 Rewind( uint64_t pos );

	public:
		void*	Alloc		( size_t size, size_t align );
		void*	Alloc		( size_t size );
		Page_s* FindPage	( size_t size ) const;
		Page_s* EnsurePage	( size_t size );
		Page_s* AppendPage	( size_t size );

	public:
		size_t Align;
		size_t PageSize;
		Array<Page_s*> Pages;
	};

	//==================================================================================

	inline
	Stack_s::Stack_s( Allocator_t alloc )
		: Align(0)
		, PageSize(0)
		, Pages(alloc)
	{}

	inline
	Stack_s::Stack_s()
		: Align(0) 
		, PageSize(0)
	{}

	inline
	Stack_s::~Stack_s()
	{ Clear(); }

	inline
	void Stack_s::Init( Allocator_t alloc, size_t page_size )
	{
		Pages.Alloc = alloc;
		PageSize = page_size;
	}

	inline
	void Stack_s::Init( Allocator_t alloc )
	{
		Pages.Alloc = alloc;
	}

	inline
	void Stack_s::Clear()
	{
		for ( int i = 0; i < Pages.Count; ++i )
			Mem_Free( Pages.Alloc, Pages[i] );
		Pages.Clear();
	}

	inline
	void Stack_s::Reset()
	{
		for ( int i = 0; i < Pages.Count; ++i )
			Pages[i]->Reset();
	}

	inline
	uint64_t Stack_s::GetPos() const
	{
		// @note:
		//	the stack is designed in a way that it will 
		//	fill in gaps below the top-most page.
		//	that means that rewinding the stack to
		//	the top position does not freed up
		//	those bits. 
		//	if this was desired we'd have to implement
		//	a strict alloc mode that does not fill
		//	holes, ie. FindPage would have to stop at
		//	the first non-empty page.
		uint64_t pos = 0;
		for ( int i = (Pages.Count-1); i >= 0; --i )
		{
			const uint64_t page = ((uint64_t)i) << 32;
			const uint32_t offs = (uint32_t)(Pages[i]->Used());
			pos = page | offs;
			if (offs > 0)
				break;
		}
		return pos;
	}

	inline
	void Stack_s::Rewind( uint64_t pos )
	{
		// decompose into page index and offset
		const  int32_t page = ( int32_t)(pos >> 32);
		const uint32_t offs = (uint32_t)(pos & UINT32_MAX);

		// get top page
		NeAssertBounds(page, Pages.Count);
		Page_s* top = Pages[page];

		// rewind top page
		NeAssert(offs <= (uint32_t)(top->End-top->Data));
		top->Pos = top->Data + offs;

		// reset pages above top page
		for ( int i = page+1; i < Pages.Count; ++i )
			Pages[i]->Reset();
	}

	inline
	void* Stack_s::Alloc( size_t size, size_t align )
	{
		const size_t full_align = align ? align : DEFAULT_ALIGN;
		NeAssert(Size_IsPow2(full_align));

		const size_t full_size = size + full_align;
		Page_s* page = EnsurePage( full_size );

		page->Pos = (uint8_t*)PtrDiff_Align_Pow2( (ptrdiff_t)page->Pos, full_align );
		NeAssert((page->End - page->Pos) >= (ptrdiff_t)size);

		void* item = page->Pos;
		page->Pos += size;

		NeAssert(page->Pos < page->End);
		NeAssert(Ptr_IsAligned_Pow2(item, full_align));
		return item;
	}

	inline
	void* Stack_s::Alloc( size_t size )
	{
		return Alloc( size, Align );
	}

	inline
	Page_s* Stack_s::FindPage( size_t size ) const
	{
		for ( int i = Pages.Count-1; i >= 0; --i )
			if ( Pages[i]->Remain() >= size )
				return Pages[i];
		return nullptr;
	}

	inline
	Page_s* Stack_s::EnsurePage( size_t size )
	{
		Page_s* page = FindPage( size );
		if (page)
			return page;

		size_t page_size = PageSize ? PageSize : DEFAULT_PAGE_SIZE;
		if (page_size < MIN_PAGE_SIZE)
			page_size = MIN_PAGE_SIZE;

		if (size > page_size)
			page_size = Size_Align_Pow2(size, MIN_PAGE_SIZE);

		return AppendPage( page_size );
	}

	inline
	Page_s* Stack_s::AppendPage( size_t size )
	{
		uint8_t* page_buffer = (uint8_t*)Mem_Alloc( Pages.Alloc, size );
		Page_s* page = (Page_s*)page_buffer;

		page->Data	= (page_buffer + sizeof(Page_s));
		page->Pos	= (page_buffer + sizeof(Page_s));
		page->End	= (page_buffer + size);

		NeAssertOut(Pages.Count != 64, "Stack exceeding 64 pages! Please increase the page size!");
		Pages.Append(page);
		return page;
	}

}

//======================================================================================
namespace nemesis 
{
	inline size_t Stack_GetSize( const Stack_s& s )
	{
		size_t total = 0;
		for ( int i = 0; i < s.Pages.Count; ++i )
			total += s.Pages[i]->Used();
		return total;
	}

	inline size_t Stack_GetCapacity( const Stack_s& s )
	{ 
		return s.PageSize * s.Pages.Count; 
	}

	inline size_t Stack_GetPageSize( const Page_s* page )
	{ 
		return Ptr_Tell( page, page->End );
	}
}

//======================================================================================
namespace nemesis 
{
	template  < typename T >
	inline T* Stack_Alloc( Stack_s& stack ) 
	{
		return (T*)(stack.Alloc( sizeof(T) ));
	}

	template  < typename T >
	inline T* Stack_Alloc( Stack_s& stack, int count ) 
	{
		return (T*)(stack.Alloc( count * sizeof(T) ));
	}

	template  < typename T >
	inline T* Stack_Calloc( Stack_s& stack ) 
	{
		return (T*)(Mem_Zero( stack.Alloc( sizeof(T) ), sizeof(T) ));
	}

	template  < typename T >
	inline T* Stack_Calloc( Stack_s& stack, int count ) 
	{
		return (T*)(Mem_Zero( stack.Alloc( count * sizeof(T) ), count * sizeof(T) ));
	}
}
