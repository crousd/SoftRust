#pragma once
#include <cstdint>

enum ItemCategory // TypeDefIndex: 10029
{
    Weapon = 0,
    Construction = 1,
    Items = 2,
    Resources = 3,
    Attire = 4,
    Tool = 5,
    Medical = 6,
    Food = 7,
    Ammunition = 8,
    Traps = 9,
    Misc = 10,
    All = 11,
    Common = 12,
    Component = 13,
    Search = 14,
    Favourite = 15,
    Electrical = 16,
    Fun = 17,
};
char* get_class_name( uint64_t address ) {
    return ( char* )( *( uint64_t* )( *( uint64_t* )( address + 0x0 ) + 0x10 ) );
}

Vector3 get_base_pos( uint64_t player ) {
    uint64_t playerModel = *( uint64_t* )( player + 0x4C0 );
    if ( !playerModel )
        return {};

    return *( Vector3* )( playerModel + 0x218 );
}

template<typename T>
class il2cpp_list_t {
public:
	uint64_t address;

	il2cpp_list_t( ) :
		address( 0 ) { }
	il2cpp_list_t( uint64_t addr ) :
		address( addr ) { }

	int size( ) {
		if ( !address )
			return 0;

		return *(int*)( address + 0x18 );
	}

	uint64_t items_array( ) {
		if ( !address )
			return 0;

		return *( uint64_t* )( address + 0x10 );
	}

	T get( int index ) {
		auto arr = items_array( );
		if ( !arr )
			return T{};

		return *( T* )( arr + 0x20 + index * sizeof( T ) );
	}
};
template<typename TKey, typename TValue>
class il2cpp_dictionary_t {
public:
	uint64_t address;

	il2cpp_dictionary_t( ) : address( 0 ) { }
	il2cpp_dictionary_t( uint64_t addr ) : address( addr ) { }

	bool valid( ) const { 
		return address != 0 ;
	}
	operator bool( ) const { 
		return valid( );
	}

	int count( ) {
		if ( !address )
			return 0;
		return *(int*)( address + 0x20 );
	}

	uint64_t entries_array( ) {
		if ( !address )
			return 0;

		return *( int* )( address + 0x18 );
	}

	static constexpr size_t entry_size( ) {
		constexpr size_t header = 8;
		constexpr size_t key_padded = sizeof( TKey ) <= 4 ? 4 :
			( sizeof( TKey ) + 7 ) & ~7;
		constexpr size_t val_padded = sizeof( TValue ) <= 4 ? 4 :
			( sizeof( TValue ) + 7 ) & ~7;

		constexpr size_t raw = header + key_padded + val_padded;
		return ( raw + 7 ) & ~7;
	}

	static constexpr size_t key_offset_in_entry( ) {
		return 8;
	}

	static constexpr size_t value_offset_in_entry( ) {
		constexpr size_t key_padded = sizeof( TKey ) <= 4 ? 4 :
			( sizeof( TKey ) + 7 ) & ~7;
		return 8 + key_padded;
	}

	TKey get_key( int index ) {
		auto entries = entries_array( );
		if ( !entries ) 
			return TKey{};

		return *( TKey* )( entries + 0x20 + index * entry_size( ) + key_offset_in_entry( ) );
	}

	TValue get_value( int index ) {
		auto entries = entries_array( );
		if ( !entries )
			return TValue{};

		return *( TValue* )( entries + 0x20 + index * entry_size( ) + value_offset_in_entry( ) );
	}

	TValue find( TKey key ) {
		auto entries = entries_array( );
		if ( !entries ) return TValue{};

		int cnt = count( );
		for ( int i = 0; i < cnt; i++ ) {
			uint64_t entry_addr = entries + 0x20 + i * entry_size( );

			int hash = *( int* )( entry_addr );
			if ( hash < 0 )
				continue;

			TKey k = *( TKey* )( entry_addr + key_offset_in_entry( ) );
			if ( k == key )
				return *( TValue* )( entry_addr + value_offset_in_entry( ) );
		}
		return TValue{};
	}

	template<typename Func>
	void for_each( Func callback ) {
		auto entries = entries_array( );
		if ( !entries )
			return;

		int cnt = count( );
		for ( int i = 0; i < cnt; i++ ) {
			uint64_t entry_addr = entries + 0x20 + i * entry_size( );

			int hash = *(int*)( entry_addr );
			if ( hash < 0 )
				continue;

			TKey k = *( TKey*)( entry_addr + key_offset_in_entry( ) );
			TValue v = *( TValue* )( entry_addr + value_offset_in_entry( ) );
			callback( k, v );
		}
	}
};

template<typename TKey, typename TValue>
class il2cpp_list_dictionary_t {
public:
	uint64_t address;

	il2cpp_list_dictionary_t( ) :
		address( 0 ) { }

	il2cpp_list_dictionary_t( uint64_t addr ) :
		address( addr ) { }

	bool valid( ) const {
		return address != 0;
	}
	operator bool( ) const {
		return valid( );
	}

	il2cpp_list_t<TKey> keys( ) {
		if ( !address )
			return {};

		return il2cpp_list_t<TKey>( *(uint64_t*)( address + 0x10 ) );
	}

	il2cpp_list_t<TValue> values( ) {
		if ( !address )
			return {};

		return il2cpp_list_t<TValue>( *( uint64_t* )( address + 0x18 ) );
	}

	int count( ) {
		return keys( ).size( );
	}

	TKey get_key( int index ) {
		return keys( ).get( index );
	}

	TValue get_value( int index ) {
		return values( ).get( index );
	}

	TValue find( TKey key ) {
		auto k = keys( );
		auto v = values( );
		int cnt = k.size( );

		for ( int i = 0; i < cnt; i++ ) {
			if ( k.get( i ) == key )
				return v.get( i );
		}
		return TValue{};
	}

	template<typename Func>
	void for_each( Func callback ) {
		auto k = keys( );
		auto v = values( );
		int cnt = k.size( );

		for ( int i = 0; i < cnt; i++ )
			callback( k.get( i ), v.get( i ) );
	}
};