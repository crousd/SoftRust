#pragma once
#include <Windows.h>
#include <iostream>
#include <thread> 
#include "math.h"
#include "gamedata.h"
#include "memory.h"

void EntityList( ) {
    auto a1 = *( uint64_t* )( memory::GameAssembly + 52119272 ); //доступ к классу получаю
    if ( !a1 )
        return;
    auto a2 = *( uint64_t* )( a1 + 0xb8 ); //0xb8 статичное значение (получаю доступ к классу)
    auto a3 = *( uint64_t* )( a2 + 0x0 );
    auto a4 = *( uint64_t* )( a3 + 0x10 );
    /*printf( "a4 = 0x%p\n" , a4 );*/
    auto a5 = *( uint64_t* )( a4 + 0x28 );
    auto size = *( int* )( a5 + 0x10 );
    /*printf( "size = %i\n", size );*/
    if ( size < 1 )
        return;
    auto buffer = *( uint64_t* )( a5 + 0x18 );
    for ( int i = 0; i < size; i++ ) {
        auto Entity = *( uint64_t* )( buffer + 0x20 + i * sizeof( uint64_t ) );
        if ( !Entity )
            continue;
        auto userId = *( uint64_t* )( Entity + 0x6C0 );
        printf( "userID = %i", userId );

        /*printf( "Entity = 0x%p\n", Entity );*/
        auto base_object = *( uint64_t* )( Entity + 0x10 );
        if ( !base_object )
            continue;

        auto object = *( uint64_t* )( base_object + 0x30 );
        if ( !object )
            continue;

        auto tag = *( uint64_t* )( object + 0x54 );
        if ( tag != 6 )
            continue;

        
       
    }

    

}


void ClientInput( uint64_t basePlayer, uint64_t state );
decltype( &ClientInput ) ClientInput_origin;
float get_Time( ) {
    return reinterpret_cast< float( * )( ) >( memory::GameAssembly + 0x20FFF30 )( );
}

void ClientInput( uint64_t basePlayer, uint64_t state ) {
    ClientInput_origin( basePlayer, state );
    uint64_t LocalPlayer = basePlayer;
    if ( !LocalPlayer ) {
        std::cout << "LocalPlayer not founded\n";
        return;
    } 
    uint64_t HeldItem = reinterpret_cast< uint64_t( * )( uint64_t ) >( memory::GameAssembly + 0x3115A0 )( LocalPlayer ); /*получения айтема в руках путем вызова игровой функции*/
    if ( !HeldItem )
        return;

    uint64_t ItemDefinition = *( uint64_t* )( HeldItem + 0x20 );//читаем память
    if ( !ItemDefinition )
        return;

    ItemCategory itemCategory = *( ItemCategory* )( ItemDefinition + 0x40 );

    uint64_t heldEntity = *( uint64_t* )( HeldItem + 0x98 );/*читаем адресс хелдайтема по оффсету*/
    if ( !heldEntity )
        return;

    auto class_name = get_class_name( heldEntity );

    if ( itemCategory != ItemCategory::Weapon )
        return;

    uint64_t recoilProperties = *( uint64_t* )( heldEntity + 0x2D8 );
    if ( !recoilProperties )
        return;

    *( float* )( recoilProperties + 0x18 ) = 0.f;
    *( float* )( recoilProperties + 0x1C ) = 0.f;
    *( float* )( recoilProperties + 0x20 ) = 0.f;
    *( float* )( recoilProperties + 0x24 ) = 0.f;
    float reloadTime = *( float* )( heldEntity + 0x2B0 );

   // *( float* )( heldEntity + 0x1F4 ) = 0.03f;
    *( float* )( heldEntity + 0x284 ) = 1.4f;
    static float last_shoot_time = 0.f;
    //*( float* )( heldEntity + 0x2B0 ) = 1.5f;

    if ( GetAsyncKeyState( 0x4E ) ) {
        uint64_t primaryMagazine = *( uint64_t* )( heldEntity + 0x2B8 );
        if ( primaryMagazine ) {
            int ammo = *( int* )( primaryMagazine + 0x1C );
            if ( ammo > 0  && get_Time() - last_shoot_time > *( float* )( heldEntity + 0x1F4 ) ) {
                reinterpret_cast< void( * )( uint64_t ) > ( memory::GameAssembly + 0x8DC7B0 )( heldEntity );
                *( int* )( primaryMagazine + 0x1C ) = ammo - 1;
                last_shoot_time = get_Time();
            }
        } 
    } 
    EntityList( );
   


}

//void ClientInput2( uint64_t movement, uint64_t input, uint64_t modelState );
//decltype ( &ClientInput2 ) ClientInput2_origin;
//
//void ClientInput2( uint64_t movement, uint64_t input, uint64_t modelState ) {
//    ClientInput2_origin(  movement,  input,  modelState );
//    
//    *( float* )( movement + 0x84 ) = 0.5f;
//
//} 
bool CanAttack( uint64_t player );
decltype ( &CanAttack ) CanAttack_origin;

bool CanAttack( uint64_t player ) {

    return true;
}








void setup_hooks( ) {
    MH_Initialize( );
    MH_CreateHook( ( void* )( memory::GameAssembly + 0x30A070 ), &ClientInput, ( void** )( &ClientInput_origin ) );
    /*MH_CreateHook( ( void* )( memory::GameAssembly + 0x94AED0 ), &ClientInput2, ( void** )( &ClientInput2_origin ) );*/
    MH_CreateHook( ( void* )( memory::GameAssembly + 0x307E60 ), &CanAttack, ( void** )( &CanAttack_origin ) );

    MH_EnableHook( MH_ALL_HOOKS );
}