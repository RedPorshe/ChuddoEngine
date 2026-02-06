#pragma once
#include "Core/Object.h"

class CWorld;
class CLevel;
class CBaseComponent;
class CSceneComponent;


class CActor : public CObject
	{
	CHUDDO_DECLARE_CLASS ( CActor, CObject )
	public:
		CActor ( CObject * owner = nullptr, const std::string & inName = "Actor" );


		virtual ~CActor ();

		virtual void BeginPlay ();
		virtual void Tick ( float deltaTime );
		virtual void EndPlay ();
		CSceneComponent * GetRootComponent () const { return RootComponent; }
		CLevel * GetLevel () const;
		CWorld * GetWorld () const;
		void SetRootComponent ( CSceneComponent * NewRoot );
		bool IsCanTickOnAttached () const { return bIsCanTickAsAttached; }
		void SetCanTickOnAttached ( bool value ) { bIsCanTickAsAttached = value; }

		bool IsAttached () const { return bIsAttached; }

		void SetIsAttached ( bool value ) { bIsAttached = value; }
	protected:
		std::vector<CBaseComponent *> ActorComponents;
		CSceneComponent * RootComponent = nullptr;
		bool bIsCanTickAsAttached { false };
		bool bIsAttached { false };
	public:

		void SetActorName ( const std::string & newName );

		template<typename Comp, typename... Args>
		Comp * AddDefaultSubObject ( const std::string & desiredDisplayName = "SubObject" );
			

	};

#include "Components/SceneComponent.h"
template<typename Comp, typename ...Args>
inline Comp * CActor::AddDefaultSubObject ( const std::string & desiredDisplayName )
	{
	static_assert( std::is_base_of<CBaseComponent, Comp>::value,
				   "Class must be derived from CBaseComponent" );
	auto newComp = this->AddSubObject<Comp> ( desiredDisplayName );

	if (RootComponent == nullptr)
		{
		if (CSceneComponent * sceneComp = dynamic_cast< CSceneComponent * >( newComp ))
			{
			RootComponent = sceneComp;
			}
		}
	ActorComponents.push_back ( newComp );
	return newComp;
	}
REGISTER_CLASS_FACTORY ( CActor );