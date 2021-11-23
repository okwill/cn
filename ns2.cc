

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/traffic-control-module.h"
#include "ns3/flow-monitor-module.h"


using namespace ns3;


int main()
{
  double simulationTime = 10; //seconds
 
  std::string socketTypeUDP="ns3::UdpSocketFactory";
  std::string socketTypeTCP="ns3::TcpSocketFactory";


 
    
  NodeContainer nodes;
  nodes.Create (4);

  PointToPointHelper pointToPoint;
  pointToPoint.SetDeviceAttribute ("DataRate", StringValue ("10Mbps"));
  pointToPoint.SetChannelAttribute ("Delay", StringValue ("2ms"));
 

   NetDeviceContainer dev02;
  dev02 = pointToPoint.Install (nodes.Get(0), nodes.Get(2));
  NetDeviceContainer dev12;
  dev12 = pointToPoint.Install (nodes.Get(1), nodes.Get(2));
  NetDeviceContainer dev23;
  dev23 = pointToPoint.Install (nodes.Get(2), nodes.Get(3));

  InternetStackHelper stack;
  stack.Install (nodes);

  
  Ipv4AddressHelper address;
  address.SetBase ("10.1.1.0", "255.255.255.0");
   Ipv4InterfaceContainer interfaces02 = address.Assign (dev02);
  address.SetBase ("10.1.2.0", "255.255.255.0");
  Ipv4InterfaceContainer interfaces12 = address.Assign (dev12);
  address.SetBase ("10.1.3.0", "255.255.255.0");
  Ipv4InterfaceContainer interfaces23 = address.Assign (dev23);

  //udp Flow
  uint16_t portudp = 7;
  Address localAddress (InetSocketAddress (Ipv4Address::GetAny (), portudp));
  PacketSinkHelper packetSinkHelper (socketTypeUDP, localAddress);
  ApplicationContainer sinkAppudp = packetSinkHelper.Install (nodes.Get (3));


  uint32_t payloadSize = 1448;
  Config::SetDefault ("ns3::TcpSocket::SegmentSize", UintegerValue (payloadSize));

  OnOffHelper onoffudp (socketTypeUDP, Ipv4Address::GetAny ());
  onoffudp.SetAttribute ("OnTime",  StringValue ("ns3::ConstantRandomVariable[Constant=1]"));
  onoffudp.SetAttribute ("OffTime", StringValue ("ns3::ConstantRandomVariable[Constant=0]"));
  onoffudp.SetAttribute ("PacketSize", UintegerValue (payloadSize));
  onoffudp.SetAttribute ("DataRate", StringValue ("50Mbps")); //bit/s
  ApplicationContainer appsudp;

  InetSocketAddress rmtudp (interfaces23.GetAddress (1), portudp);
  ;
  AddressValue remoteAddressUDP (rmtudp);
  onoffudp.SetAttribute ("Remote", remoteAddressUDP);
  appsudp.Add (onoff.Install (nodes.Get (0)));
  appsudp.Start (Seconds (1.0));
  appsudp.Stop (Seconds (simulationTime + 0.1));

  //tcp flow
   uint16_t porttcp = 9;
  Address localAddresstcp (InetSocketAddress (Ipv4Address::GetAny (), porttcp));
  PacketSinkHelper packetSinkHelpertcp (socketTypetcp, localAddresstcp);
  ApplicationContainer sinkApptcp = packetSinkHelper.Install (nodes.Get (3));

  sinkApp.Start (Seconds (0.5));
  sinkApp.Stop (Seconds (simulationTime + 0.1));

 
  Config::SetDefault ("ns3::TcpSocket::SegmentSize", UintegerValue (payloadSize));

  OnOffHelper onofftcp (socketTypeTCP, Ipv4Address::GetAny ());
  onofftdp.SetAttribute ("OnTime",  StringValue ("ns3::ConstantRandomVariable[Constant=1]"));
  onofftdp.SetAttribute ("OffTime", StringValue ("ns3::ConstantRandomVariable[Constant=0]"));
  onofftdp.SetAttribute ("PacketSize", UintegerValue (payloadSize));
  onofftdp.SetAttribute ("DataRate", StringValue ("50Mbps")); //bit/s
  ApplicationContainer appstdp;

  InetSocketAddress rmttdp (interfaces23.GetAddress (1), portudp);
  
  AddressValue remoteAddressTcP (rmttdp);
  onofftdp.SetAttribute ("Remote", remoteAddresstcP);
  appstdp.Add (onoff.Install (nodes.Get (0)));
  appstdp.Start (Seconds (1.5));
  appstdp.Stop (Seconds (simulationTime + 0.1));


  FlowMonitorHelper flowmon;
  Ptr<FlowMonitor> monitor = flowmon.InstallAll();

  Simulator::Stop (Seconds (simulationTime + 5));
  Simulator::Run ();

  Ptr<Ipv4FlowClassifier> classifier = DynamicCast<Ipv4FlowClassifier> (flowmon.GetClassifier ());
  std::map<FlowId, FlowMonitor::FlowStats> stats = monitor->GetFlowStats ();
  std::cout << std::endl << "*** Flow monitor statistics ***" << std::endl;
 

  Simulator::Destroy ();
for(auto it = stats.begin(); it!=stats.end(); it++){
   Ipv4FlowClassifier::FiveTuple t = classifier->FindFlow(it->first);
   std::cout<<"Flow ID: "<<it->first<<"\n";
   std::cout<<"Source address: "<<t.sourceAddress<<"\n";
   std::cout<<"Destination address: "<<t.destinationAddress<<"\n";
   std::cout<<"Tx Packets: "<<it->second.txPackets<<"\n";
   std::cout<<"Rx Packets: "<<it->second.rxPackets<<"\n";
   std::cout<<"Lost Packets: "<<it->second.lostPackets<<"\n";
   std::cout <<"Throughput: " <<it->second.rxBytes * 8.0 / (it->second.timeLastRxPacket.GetSeconds () - it->second.timeFirstRxPacket.GetSeconds ()) / 1000000 << " Mbps" << std::endl;
  }
  


    return 0;
}
