#include "ns3/core-module.h" 
#include "ns3/network-module.h" 
#include "ns3/internet-module.h" 
#include "ns3/point-to-point-module.h" 
#include "ns3/applications-module.h" 
#include "ns3/traffic-control-module.h" 
#include "ns3/flow-monitor-module.h" 
 
using namespace ns3; 
 
int main () 
{ 
  double simulationTime = 10; //seconds 
  std::string socketType="ns3::UdpSocketFactory";//"ns3::TcpSocketFactory"; 
 
  NodeContainer nodes; 
  nodes.Create (4); 
 
  PointToPointHelper p2p; 
  p2p.SetDeviceAttribute ("DataRate", StringValue ("10Mbps")); 
  p2p.SetChannelAttribute ("Delay", StringValue ("2ms")); 
  p2p.SetQueue ("ns3::DropTailQueue", "MaxSize", StringValue ("1p")); 
 
  NetDeviceContainer dev02; 
  dev02= p2p.Install (nodes.Get(0),nodes.Get(2)); 
  NetDeviceContainer dev12; 
  dev12= p2p.Install (nodes.Get(1),nodes.Get(2)); 
  NetDeviceContainer dev23; 
  dev23= p2p.Install (nodes.Get(2),nodes.Get(3)); 
  InternetStackHelper stack; 
  stack.Install (nodes); 
   
  Ipv4AddressHelper address; 
 
  address.SetBase ("10.1.1.0", "255.255.255.0"); 
  Ipv4InterfaceContainer interfaces02 = address.Assign (dev02); 
 
  address.SetBase ("10.1.2.0", "255.255.255.0"); 
  Ipv4InterfaceContainer interfaces12 = address.Assign (dev12); 
  
  address.SetBase ("10.1.3.0", "255.255.255.0"); 
  Ipv4InterfaceContainer interfaces23 = address.Assign (dev23); 
   
  
  Ipv4GlobalRoutingHelper::PopulateRoutingTables ();  
 
  //UDP Flow 
  uint16_t port = 7; 
  Address localAddress (InetSocketAddress (Ipv4Address::GetAny (), port)); 
  PacketSinkHelper psh (socketType, localAddress); 
  ApplicationContainer sinkApp = psh.Install (nodes.Get (3)); 
  sinkApp.Start (Seconds (0.0)); 
  sinkApp.Stop (Seconds (simulationTime + 0.1)); 
 
  OnOffHelper onoff (socketType, Ipv4Address::GetAny ()); 
  onoff.SetAttribute ("OnTime",  StringValue ("ns3::ConstantRandomVariable[Constant=1]")); 
  onoff.SetAttribute ("OffTime", StringValue ("ns3::ConstantRandomVariable[Constant=0]")); 
  onoff.SetAttribute ("DataRate", StringValue ("50Mbps")); //bit/s 
  ApplicationContainer apps; 
 
  InetSocketAddress rmt (interfaces23.GetAddress (1), port); 
  AddressValue remoteAddress (rmt); 
  onoff.SetAttribute ("Remote", remoteAddress); 
  apps.Add (onoff.Install (nodes.Get (0))); 
  apps.Start (Seconds (1.0)); 
  apps.Stop (Seconds (simulationTime + 0.1)); 
 
  // TCP FLOW
  socketType="ns3::TcpSocketFactory";
  uint16_t port1 = 9; 
  Address localAddress1 (InetSocketAddress (Ipv4Address::GetAny (), port1)); 
  PacketSinkHelper pshtcp (socketType, localAddress1); 
  ApplicationContainer sinkApptcp = pshtcp.Install (nodes.Get (3)); 
  sinkApptcp.Start (Seconds (0.01)); 
  sinkApptcp.Stop (Seconds (simulationTime + 0.1)); 
 
  OnOffHelper onofftcp (socketType, Ipv4Address::GetAny ()); 
  onofftcp.SetAttribute ("OnTime",  StringValue ("ns3::ConstantRandomVariable[Constant=1]")); 
  onofftcp.SetAttribute ("OffTime", StringValue ("ns3::ConstantRandomVariable[Constant=0]")); 
  onofftcp.SetAttribute ("DataRate", StringValue ("50Mbps")); //bit/s 
  ApplicationContainer appstcp; 
 
  InetSocketAddress rmttcp (interfaces23.GetAddress (1), port); 
  AddressValue remoteAddresstcp (rmttcp); 
  onoff.SetAttribute ("Remote", remoteAddress); 
  appstcp.Add (onoff.Install (nodes.Get (1))); 
  appstcp.Start (Seconds (1.1)); 
  appstcp.Stop (Seconds (simulationTime + 0.1));  

  FlowMonitorHelper flowmon; 
  Ptr<FlowMonitor> monitor = flowmon.InstallAll(); 
 
  Simulator::Stop (Seconds (simulationTime + 5)); 
  Simulator::Run (); 
 
  Ptr<Ipv4FlowClassifier> classifier = DynamicCast<Ipv4FlowClassifier> (flowmon.GetClassifier ()); 
  std::map<FlowId, FlowMonitor::FlowStats> stats = monitor->GetFlowStats (); 
  std::cout << std::endl << "*** Flow monitor statistics ***"  << std::endl; 
 
  for (std::map<FlowId, FlowMonitor::FlowStats>::const_iterator iter = stats.begin (); iter != stats.end 
(); ++iter)  
    {  
      Ipv4FlowClassifier::FiveTuple t = classifier->FindFlow (iter->first);  
      std::cout << "Flow ID: " << iter->first << " Src Addr " << t.sourceAddress << " Dst Addr " << 
t.destinationAddress<< std::endl;  
      std::cout << "Tx Packets   = " << iter->second.txPackets<< std::endl;  
      std::cout << "Rx Packets   = " << iter->second.rxPackets<< std::endl;  
      std::cout << "Lost Packets = " << iter->second.lostPackets<< std::endl;  
      std::cout << "Throughput   = " << iter->second.rxBytes * 8.0 / (iter->second.timeLastRxPacket.GetSeconds()-iter->second.timeFirstTxPacket.GetSeconds()) / 1000000  
<< " Kbps"<< std::endl;  
    } 
 
  Simulator::Destroy (); 
 
  return 0; 
} 
 
 
/* 
Usage of the existing examples: (Lab1.cc--→ example/traffic-control/traffic-control.cc) 
 
*/
