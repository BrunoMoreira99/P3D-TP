#pragma once

#include "Roose/Core/Time.h"
#include "Roose/Events/Event.h"

namespace Roose {

	class Layer
	{
	public:
		Layer(const std::string& name = "Layer")
			: m_DebugName(name) {}
		virtual ~Layer() = default;

		virtual void OnAttach() {}
		virtual void OnDetach() {}
		virtual void OnUpdate(Timestep ts) {}
		virtual void OnEvent(Event& event) {}

		[[nodiscard]] const std::string& GetName() const { return m_DebugName; }
	protected:
		std::string m_DebugName;
	};

}
