#pragma once

#include "Components/BaseComponent.h"
#include "Core/Collision.h"  // Добавляем заголовок коллизий

class CBaseCollisionComponent : public CBaseComponent
    {
    CHUDDO_DECLARE_CLASS ( CBaseCollisionComponent, CBaseComponent );

    public:
        CBaseCollisionComponent ( CObject * inOwner = nullptr,
                                  const std::string & inDisplayName = "BaseCollisionComponent" );
        virtual ~CBaseCollisionComponent ();

        virtual void InitComponent () override;
        virtual void Tick ( float DeltaTime ) override;
        virtual void OnBeginPlay () override;

        // Основные методы коллизий
        virtual bool CheckCollision ( CBaseCollisionComponent * other ) const;
        FVector GetWorldLocation () ;
        // Управление коллизиями
        void SetCollisionEnabled ( bool value = true ) { bIsCollisionEnabled = value; }
        bool IsCollisionEnabled () const { return bIsCollisionEnabled; }

        // Работа с каналами (новые методы)
        void SetCollisionChannel ( const FCollisionChannel & channel );
        void SetCollisionChannel ( const std::string & channelName );  // Перегрузка для имени
        const FCollisionChannel & GetCollisionChannel () const { return m_CollisionChannel; }

        // Быстрые настройки каналов (удобные методы)
        void SetChannelAsStatic ();
        void SetChannelAsDynamic ();
        void SetChannelAsCharacter ();
        void SetChannelAsTrigger ();
        void SetChannelAsPawn ();
        void SetChannelAsVehicle ();
        void SetChannelAsInteractable ();
        void SetChannelAsCustom ( const std::string & channelName,
                                  ECollisionResponse defaultResponse = ECollisionResponse::BLOCK );

           // Настройка ответов на другие каналы
        void SetResponseToChannel ( const std::string & otherChannelName,
                                    ECollisionResponse response );
        void SetResponseToChannel ( ECollisionChannel otherChannel,
                                    ECollisionResponse response );

           // Проверка взаимодействия с другим компонентом
        bool CanCollideWith ( const CBaseCollisionComponent * other ) const;
        bool CanCollideWith ( const std::string & otherChannelName ) const;
        bool ShouldBlockWith ( const CBaseCollisionComponent * other ) const;
        bool ShouldOverlapWith ( const CBaseCollisionComponent * other ) const;
        virtual float GetCollisionRadius () const { return 0.0f; }
        // Форма коллизии
        ECollisionShape GetShapeType () const { return m_CollisionShape; }

        void OnBeginOverlap ( CBaseCollisionComponent * other );
        void OnEndOverlap ( CBaseCollisionComponent * other );

    protected:
        // Установка формы (только для наследников)
        void SetShapeType ( const ECollisionShape & inShape );
        std::unordered_set<CBaseCollisionComponent *> OverlappingComponents;
        // Данные коллизии
        FCollisionChannel m_CollisionChannel;
        ECollisionShape m_CollisionShape = ECollisionShape::NONE;
        bool bIsCollisionEnabled = true;
    };

REGISTER_CLASS_FACTORY ( CBaseCollisionComponent );