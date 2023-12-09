#pragma once

#include <map>

namespace that
{
	class Graph final
	{
	public:
		Graph() = default;
		~Graph() = default;

		/// <summary>
		/// <para>Add a node to the graph</para> 
		/// <para>x and y values need to be between [0,1]</para> 
		/// </summary>
		void AddNode(float x, float y);

		/// <summary>
		/// <para>Returns the y-value on the graph [0,1] for a given x value</para> 
		/// <para>The given x value needs to be between [0,1]</para> 
		/// <para>A graph needs to have lower (x=0) and upper (x=1) bound nodes to succesfully get a value.</para> 
		/// </summary>
		float GetValue(float x) const;

	private:
		std::map<float, float> m_Nodes{};
	};
}
