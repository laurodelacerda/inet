//
// Copyright (C) 2016 OpenSim Ltd.
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program; if not, see <http://www.gnu.org/licenses/>.
//

package inet.visualizer.networklayer;

import inet.visualizer.contract.INetworkRouteVisualizer;

//
// This module integrates canvas and osg network route visualizers into a single module.
//
// @see ~NetworkRouteCanvasVisualizer, ~NetworkRouteOsgVisualizer, ~RouteVisualizerBase,
// and ~INetworkRouteVisualizer
//
module NetworkRouteVisualizer like INetworkRouteVisualizer
{
    parameters:
        @display("i=block/app_s");
        string canvasVisualizerType = default("NetworkRouteCanvasVisualizer");
        string osgVisualizerType = default("NetworkRouteOsgVisualizer");
    submodules:
        canvasVisualizer: <canvasVisualizerType> like INetworkRouteVisualizer if canvasVisualizerType != "" {
            parameters:
                @display("p=100,50");
        }
        osgVisualizer: <osgVisualizerType> like INetworkRouteVisualizer if osgVisualizerType != "" {
            parameters:
                @display("p=100,150");
        }
}
