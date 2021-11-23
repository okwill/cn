


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
 
      std::string socketType;
  socketType = "ns3::UdpSocketFactory";
    

  NodeContainer nodes;
  nodes.Create (3);

  PointToPointHelper pointToPoint;
  pointToPoint.SetDeviceAttribute ("DataRate", StringValue ("10Mbps"));
  pointToPoint.SetChannelAttribute ("Delay", StringValue ("2ms"));
  pointToPoint.SetQueue ("ns3::DropTailQueue", "MaxSize", StringValue ("1p"));

 
  NetDeviceContainer dev01;
  dev01 = pointToPoint.Install (nodes.Get(0),nodes.Get(1));
  NetDeviceContainer dev12;
  dev12 = pointToPoint.Install (nodes.Get(1), nodes.Get(2));



  InternetStackHelper stack;
  stack.Install (nodes);

  

  Ipv4AddressHelper address;
  address.SetBase ("10.1.1.0", "255.255.255.0");
  Ipv4InterfaceContainer interfaces01 = address.Assign (dev01);
  address.SetBase ("10.1.2.0", "255.255.255.0");
  Ipv4InterfaceContainer interfaces12 = address.Assign (dev12);

  Ipv4GlobalRoutingHelper::PopulateRoutingTables();

  //Flow
  uint16_t port = 7;
  Address localAddress (InetSocketAddress (Ipv4Address::GetAny (), port));
  PacketSinkHelper packetSinkHelper (socketType, localAddress);
  ApplicationContainer sinkApp = packetSinkHelper.Install (nodes.Get (2));

  sinkApp.Start (Seconds (0.0));
  sinkApp.Stop (Seconds (simulationTime + 0.1));

  

  OnOffHelper onoff (socketType, Ipv4Address::GetAny ());
  onoff.SetAttribute ("OnTime",  StringValue ("ns3::ConstantRandomVariable[Constant=1]"));
  onoff.SetAttribute ("OffTime", StringValue ("ns3::ConstantRandomVariable[Constant=0]"));
  
  onoff.SetAttribute ("DataRate", StringValue ("50Mbps")); //bit/s
  ApplicationContainer apps;

  InetSocketAddress rmt (interfaces12.GetAddress (1), port);
  
  AddressValue remoteAddress (rmt);
  onoff.SetAttribute ("Remote", remoteAddress);
  apps.Add (onoff.Install (nodes.Get (0)));
  apps.Start (Seconds (1.0));
  apps.Stop (Seconds (simulationTime + 0.1));

  FlowMonitorHelper flowmon;
  Ptr<FlowMonitor> monitor = flowmon.InstallAll();

  Simulator::Stop (Seconds (simulationTime + 5));
  Simulator::Run ();

  Ptr<Ipv4FlowClassifier> classifier = DynamicCast<Ipv4FlowClassifier> (flowmon.GetClassifier ());
  std::map<FlowId, FlowMonitor::FlowStats> stats = monitor->GetFlowStats ();
  std::cout << std::endl << "*** Flow monitor statistics ***" << std::endl;
  for(auto it = stats.begin(); it!=stats.end(); it++){
     Ipv4FlowClassifier::FiveTuple t = classifier->FindFlow(it->first);
     std::cout<<"Flow ID: "<<it->first<<"\n";
     std::cout<<"Source address: "<<t.sourceAddress<<"\n";
    std::cout<<"Tx Packets: "<<it->second.txPackets<<"\n";
   std::cout<<"Rx Packets: "<<it->second.rxPackets<<"\n";
   std::cout<<"Lost Packets: "<<it->second.lostPackets<<"\n";
    std::cout <<"Throughput: " <<it->second.rxBytes * 8.0 / (it->second.timeLastRxPacket.GetSeconds () - it->second.timeFirstRxPacket.GetSeconds ()) / 1000000 << " Mbps" << std::endl;
 }



 
  Simulator::Destroy ();

  
  return 0;
}
