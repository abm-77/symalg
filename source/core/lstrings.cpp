#pragma region CHARACTER

internal i32 CharToDigit (char c) {
    return c - '0';
}

internal char CharIsUpper (char c) {
    return c >= 'A' && c <= 'Z';
}

internal char CharIsLower (char c) {
    return c >= 'a' && c <= 'z';
}

internal char CharToUpper (char c) {
    char result = c;
    if (!CharIsUpper(c)) {
        result = c + 'A' - 'a';
    } 

    return result;
}

internal char CharToLower (char c) {
    char result = c;
    if (!CharIsLower(c)) {
        result = c + 'a' - 'A';
    } 

    return result;
}

internal b32 CharIsAlpha (char c) {
    return CharIsLower(c) || CharIsUpper(c);
}

internal b32 CharIsDigit (char c) {
    return c >= '0' && c <= '9';
}

internal b32 CharIsAlphaNum(char c) {
    return CharIsAlpha(c) || CharIsDigit(c);
}

internal b32 CharIsSpace(char c) {
    return c == ' ' || c == '\t';
}


#pragma endregion

#pragma region STRING


internal b32 StringValid (String str) {
    return str.size != 0 && str.data;
}

internal b32 StringIsAlpha (String str) {
	return 	StringValid(str) 	&& 
			str.size == 1 		&&  
			CharIsAlpha(str.data[0]);
}

internal i32 StringCompare (String str1, String str2) {
    int min_length = Min(str1.size, str2.size);
    return MemoryCompare (str1.data, str2.data, min_length);
}

internal b32 StringMatch (String str1, String str2) {
    if (str1.size != str2.size) return false;
    return MemoryCompare (str1.data, str2.data, str1.size) == 0;
}

internal b32 StringMatchPrefix (String str, String prefix) {
    if (str.size < prefix.size) return false;
    return MemoryCompare (str.data, prefix.data, prefix.size) == 0;
}

internal b32 StringMatchSuffix (String str, String suffix) {
    if (str.size < suffix.size) return false;
    u32 offset = str.size - suffix.size;
    return MemoryCompare(str.data + offset, suffix.data, suffix.size) == 0;
}

internal char* StringToCString(String string, M_Arena* arena) {
	char* cstr =  (char*) M_ArenaPushZero(arena, string.size + 1);
	MemoryCopy(cstr, string.data, string.size);
	cstr[string.size] = '\0';
	return cstr;
}

internal String StringSubstring(String string, size_t begin, size_t end) {
    String result = {0};

    if (begin < 0) {
        begin = string.size + begin;
    }

    if (end <= 0) {
        end = string.size + end;
    }

    if (begin >= 0 && begin < end && end <= string.size) {
        result.data = string.data + begin;
        result.size = end - begin;
    }
    return result;
}

internal i32 StringFindFirst (String string, String pattern) {
    if (pattern.size < string.size) {
        u32 index = 0;

        for (String substring = string; substring.size >= pattern.size; substring = StringSubstring(substring, 1, 0)) {
            if (StringMatchPrefix(substring, pattern)) {
                return index;
            }

            index += 1;
        }
    }

    return -1;
}

internal i32 StringFindLast (String string, String pattern) {
    if (pattern.size < string.size) {
        u32 index = string.size - 1;

        for (String substring = string; StringValid(substring); substring = StringSubstring(substring, 0, -1)) {
            if (StringMatchSuffix(substring, pattern)) {
                return index;
            }

            index -= 1;
        }
    }

    return -1;
}

internal b32 StringContains (String string, String pattern) {
    return StringFindFirst(string, pattern) != -1;
}

internal String StringAdvance(String string, i32 amount) {
    return StringSubstring(string, amount, string.size);
}

internal String StringEatSpaces (String string) {
    while (StringValid(string) && CharIsSpace(CharAt(string, 0))) {
        string = StringAdvance(string, 1); 
    }

    return string;
}

internal String StringPopFirstSplit(String* string, String split_by) {
    String result = {0};

    i32 index = StringFindFirst(*string, split_by);
    if (index != -1) {
        result.data   = string->data;
        result.size   = index;
        string->data  += index + split_by.size;
        string->size -= index + split_by.size;
    }
    else {
        result  = *string;
        *string = String{0};
    }

    return result;
}

internal String StringPopLast(String* string, String split_by) {
    String result = {0};

    i32 index = StringFindLast(*string, split_by);
    if (index != -1) {
        result.data     = string->data + index;
        result.size   = string->size - index;
        string->size -= string->size - index - split_by.size;
    }
    else {
        result  = *string;
        *string = String{0};
    }

    return result;
}

internal String StringCopy (String string, M_Arena* arena) {
	String result = {0};
	char* cstr = StringToCString(string, arena); 
	result.data = cstr;
	result.size = string.size;
	return result;
}

internal u64 StringCount (String string, String pattern) {
	u64 count = 0;

	while (StringContains(string, pattern)) {
		StringPopFirstSplit(&string, pattern);
		count++;
	}

	return count;
}

// NOTE(bryson): currently makes a copy of the original string, maybe change?
internal String StringToUpper (String string, M_Arena* arena) {
	String result = StringCopy(string, arena);
	for (u32 i = 0; i < string.size; ++i) {
		// NOTE(bryson): assumes string is composes of only alpha chars, maybe check this?
		char* c = &result.data[i];
		*c &= CharToUpper(*c);
	}
	return result;
}

// NOTE(bryson): currently makes a copy of the original string, maybe change?
internal String StringToLower (String string, M_Arena* arena) {
	String result = StringCopy(string, arena);
	for (u32 i = 0; i < string.size; ++i) {
		// NOTE(bryson): assumes string is composes of only alpha chars, maybe check this?
		char* c = &result.data[i];
		*c &= CharToLower(*c);
	}
	return result;
}

#pragma endregion

#pragma region STRING_BUILDER
internal inline u32 StringBuilderRemainingCapacity (const StringBuilder* builder) {
    return builder->capacity - builder->size;
}

internal void StringBuilderReserve (StringBuilder* builder, u32 new_capacity) {
    if (new_capacity > builder->capacity) {
        char* new_buffer = (char*) M_ArenaRealloc(builder->arena, builder->data, builder->capacity, new_capacity);
        if (new_buffer) {
            builder->data = new_buffer;
            builder->valid = true;
        }
        else {
            builder->valid = false;
        }
    }
}

internal void StringBuilderEnsureCapacityFor (StringBuilder* builder, u32 size) {
    u32 remaining_capacity = StringBuilderRemainingCapacity(builder);
    if (remaining_capacity < size) {
        u32 amount_to_reserve = Max(builder->capacity * 2, builder->capacity + (size - remaining_capacity) + 1);
        StringBuilderReserve(builder, amount_to_reserve);
    }
}

internal String StringBuilderToString (StringBuilder builder) {
    String result = {0};
    if (builder.valid) {
        result.data = builder.data;
        result.size = builder.size;
    }

    return result;
}

// NOTE(bryson): you can specifiy an allocator to this if you want, arena = NULL uses the builder's builtin arena
internal String StringBuilderToStringEx (StringBuilder builder, M_Arena* arena) {
    String result = {0};
    if (builder.valid) {
		result.data = StringToCString(MakeString(builder.data), (arena == NULL) ? builder.arena : arena);
        result.size = builder.size;
    }
    return result;
}

internal StringBuilder StringBuilderCreate (M_Arena* arena, u32 initial_amount) {
    StringBuilder result = {0};

    void* data = M_ArenaPushZero(arena, initial_amount);
    if (data) {
        result.data = (char*) data;
        result.capacity = initial_amount;
        result.valid = true;
		result.arena = arena;
    }

    return result;
}

internal void StringBuilderClear(StringBuilder* builder) {
    MemoryZero(builder->data, sizeof(builder->data));
    builder->size = 0;
}

internal void StringBuilderAppend (StringBuilder* builder, String str) {
    StringBuilderEnsureCapacityFor(builder, str.size);
    MemoryCopy(builder->data + builder->size, str.data, str.size);
    builder->size += str.size;
    builder->data[builder->size] = 0;
}

internal void StringBuilderPrepend (StringBuilder* builder, String str) {
    StringBuilderEnsureCapacityFor(builder, str.size);

    MemoryMove(builder->data + str.size, builder->data, builder->size);
    MemoryCopy(builder->data, str.data, str.size);

    builder->size += str.size;
    builder->data[builder->size] = 0;
}

// NOTE(bryson): you can specifiy an allocator to this if you want, arena = NULL uses the builder's builtin arena
internal StringBuilder StringBuilderClone (StringBuilder builder_to_clone, M_Arena* arena) {
    StringBuilder result = {0};    

    if (builder_to_clone.valid) {
		M_Arena* arena_to_use = (arena == NULL) ? builder_to_clone.arena : arena;
        result = StringBuilderCreate(arena_to_use, builder_to_clone.capacity);
        StringBuilderAppend(&result, StringBuilderToStringEx(builder_to_clone, arena_to_use)); // makes separate copy
    }

    return result;
}

internal void StringBuilderInsert (StringBuilder* builder, String str, u32 at) {
    if (StringValid(str) && at > 0) {
        StringBuilderEnsureCapacityFor(builder, str.size);

        // Move data at insetion point ahead
        {
            char* destination = builder->data + at + str.size; 
            char* source = builder->data + at;
            u32 source_size = builder->size - at;
            MemoryMove(destination, source, source_size);
        }

        {
            char* destination = builder->data + at;
            MemoryCopy(destination, str.data, str.size);
        }

        builder->size += str.size;
        builder->data[builder->size] = 0;
    }
}

/* Memory is managed by arena
internal void StringBuilderFree (StringBuilder* builder) {
    free(builder->data);
    builder->size = 0;
    builder->capacity = 0;
}
*/
#pragma endregion