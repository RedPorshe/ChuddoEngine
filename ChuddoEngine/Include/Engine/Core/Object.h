#pragma once
#include "CoreMinimal.h"
#include <atomic>
#include <random>
#include <sstream>
#include <iomanip>

// Forward declaration
class CObjectFactory;

// Base macros for class declaration - УПРОЩЕННЫЕ
#define CHUDDO_DECLARE_CLASS(ClassName, BaseClassName) \
public: \
    using Super = BaseClassName; \
    static const char* StaticClassName() { return #ClassName; } \
    static const char* StaticBaseClassName() { return #BaseClassName; } \
    virtual const char* GetObjectClassName() const { return #ClassName; }
//namespace\
//	{\
//	struct CObjectRegistrar\
//		{\
//		CObjectRegistrar () {\
//			CObjectFactory::GetInstance ().RegisterClass<ClassName> ();\
//			}\
//		};\
//	static CObjectRegistrar CObject_AutoReg;\
//	}\


#define CHUDDO_DECLARE_ABSTRACT_CLASS(ClassName, BaseClassName) \
public: \
    using Super = BaseClassName; \
    static const char* StaticClassName() { return #ClassName; } \
    static const char* StaticBaseClassName() { return #BaseClassName; } \
    virtual const char* GetObjectClassName() const { return #ClassName; }\


class CObject
	{
	CHUDDO_DECLARE_ABSTRACT_CLASS ( CObject, CObject )

	private:
		// Thread-safe UUID generator
		static std::string GenerateUUID ()
			{
			static std::random_device rd;
			static std::mt19937_64 gen ( rd () );
			static std::uniform_int_distribution<uint64_t> dis;

			uint64_t part1 = dis ( gen );
			uint64_t part2 = dis ( gen );

			std::stringstream ss;
			ss << std::hex << std::setfill ( '0' )
				<< std::setw ( 16 ) << part1
				<< std::setw ( 16 ) << part2;
			return ss.str ();
			}

	public:
		CObject ( CObject * inOwner = nullptr, const std::string & inDisplayName = "Object" );
		virtual ~CObject ();

		// Basic getters
		CObject * GetOwner () const { return ObjectOwner; }
		bool HasOwner () const { return GetOwner () != nullptr; }

		// Display name (for UI/Editor) - can be changed
		std::string GetName () const { return DisplayName; }

		// Unique persistent identifier (UUID) - never changes
		std::string GetUUID () const { return ObjectUUID; }


		// For backward compatibility
		std::string GetUniqName () const { return DisplayName + "_" + GetShortUUID (); }
		std::string GetFullIdentifier () const { return GetUniqName (); }

		void UpdateDebugIdentifier ();
		std::string GetShortUUID () const
			{
			return ( ObjectUUID.length () > 8 ) ? ObjectUUID.substr ( 0, 8 ) : ObjectUUID;
			}

			// Object search
		CObject * FindOwned ( const std::string & displayName ) const;
		const std::vector<std::unique_ptr<CObject>> & GetOwnedObjects () const { return OwnedObjects; }
		size_t GetNumOwnedObjects () const { return OwnedObjects.size (); }
		bool HasOwnedObjects () const { return !OwnedObjects.empty (); }

		// Recursive search by display name
		bool FindRecursive ( const std::string & displayName );
		CObject * FindObjectByDisplayNameRecursive ( const std::string & displayName );

		// Search by UUID (unique)
		CObject * FindByUUID ( const std::string & uuid ) const;
		CObject * FindByUUIDRecursive ( const std::string & uuid );

		// Object management
		bool RemoveOwnedObject ( const std::string & displayName );
		void AddOwnedObject ( std::unique_ptr<CObject> object );
		void AddOwnedObject ( CObject * object );
		bool TransferOwnership ( CObject * obj, CObject * newOwner );

		// Child object creation - ALWAYS successful (auto-name generation)
		// Version 1: Direct creation (old way, still works)
		template<typename ClassName, typename... Args>
		ClassName * AddSubObject ( const std::string & desiredDisplayName = "SubObject", Args&&... args )
			{
			static_assert( std::is_base_of<CObject, ClassName>::value,
						   "Class must be derived from CObject" );

			if (desiredDisplayName.empty ())
				{
				std::cerr << "Error: Object display name cannot be empty!\n";
				return nullptr;
				}

				// Find hierarchy root for global check
			CObject * root = this;
			while (root->GetOwner ())
				{
				root = root->GetOwner ();
				}

			std::string finalDisplayName = desiredDisplayName;

			// Check if display name already exists globally
			if (root->FindRecursive ( desiredDisplayName ))
				{
					// Display name exists ANYWHERE in hierarchy, generate unique variant
				finalDisplayName = GenerateUniqueDisplayNameVariant ( desiredDisplayName, root );
				std::cout << "Note: Display name '" << desiredDisplayName
					<< "' already exists, using '" << finalDisplayName << "' instead\n";
				}

				// Create object (old way - direct construction)
			auto newObj = std::make_unique<ClassName> ( this, finalDisplayName, std::forward<Args> ( args )... );
			ClassName * rawPtr = newObj.get ();

			AddOwnedObject ( std::move ( newObj ) );

			return rawPtr;
			}

			// Version 2: Factory creation (new way)
		CObject * AddSubObjectByClass ( const std::string & className,
										const std::string & desiredDisplayName = "SubObject" );

		   // Renaming operations
		bool RenameOwnedObject ( const std::string & oldDisplayName, const std::string & newDisplayName );
		bool Rename ( const std::string & newDisplayName );

		// Safe type casting for search
		template<typename T>
		T * FindOwnedAs ( const std::string & displayName ) const
			{
			CObject * obj = FindOwned ( displayName );
			if (obj)
				{
				return dynamic_cast< T * >( obj );
				}
			return nullptr;
			}

			// Cloning
		std::unique_ptr<CObject> Clone () const;

		// Get root object
		CObject * GetRoot () const
			{
			const CObject * root = this;
			while (root->GetOwner ())
				{
				root = root->GetOwner ();
				}
			return const_cast< CObject * >( root );
			}

	protected:
		CObject * ObjectOwner = nullptr;
		std::string DisplayName {};        // User-friendly name (for display/editor)
		std::string ObjectUUID {};         // Unique immutable identifier
		std::vector<std::unique_ptr<CObject>> OwnedObjects;
		friend class CObjectFactory;

		// Generate unique display name variant
		static std::string GenerateUniqueDisplayNameVariant ( const std::string & baseDisplayName, CObject * root );

		// Find all objects with similar display names
		static void CollectSimilarDisplayNames ( CObject * node, const std::string & baseDisplayName,
												 std::vector<std::string> & result );

		   // Get short version of UUID for display
		static std::string GetShortUUID ( const std::string & uuid )
			{
			if (uuid.length () > 8)
				return uuid.substr ( 0, 8 );
			return uuid;
			}

			// Registration helper - ДОБАВЛЕНО
		template<typename T>
		static bool RegisterClassHelper ()
			{
			return true;
			}
	};
#include "Core/ObjectFactory.h"