// ndn-simple.cpp

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/ndnSIM-module.h"

namespace ns3 {

int
main(int argc, char* argv[])
{
//设置点对点连接、信道默认参数
  Config::SetDefault("ns3::PointToPointNetDevice::DataRate", StringValue("1Mbps"));
  Config::SetDefault("ns3::PointToPointChannel::Delay", StringValue("10ms"));
  Config::SetDefault("ns3::DropTailQueue::MaxPackets", StringValue("20"));

//使用命令行./waf
  CommandLine cmd;
  cmd.Parse(argc, argv);

  NodeContainer nodes;
  nodes.Create(3);

//连接节点
  PointToPointHelper p2p;
  p2p.Install(nodes.Get(0), nodes.Get(1));
  p2p.Install(nodes.Get(1), nodes.Get(2));

//安装协议栈，采用默认方式
  ndn::StackHelper ndnHelper;
  ndnHelper.SetDefaultRoutes(true);
  ndnHelper.InstallAll();

//选择转发策略，这里是为所有节点选择multicast多播策略
  ndn::StrategyChoiceHelper::InstallAll("/prefix", "/localhost/nfd/strategy/multicast");

  ndn::AppHelper consumerHelper("ns3::ndn::ConsumerCbr");
//兴趣包前缀
  consumerHelper.SetPrefix("/prefix");
//发送频率 每秒10个包
  consumerHelper.SetAttribute("Frequency", StringValue("10")); 
//只从0号节点发送
  consumerHelper.Install(nodes.Get(0));         

//设置内容生产者
  ndn::AppHelper producerHelper("ns3::ndn::Producer");
  // Producer will reply to all requests starting with /prefix
//生产者会回复所有前缀对应的兴趣包
  producerHelper.SetPrefix("/prefix");
//有效载荷1024字节
  producerHelper.SetAttribute("PayloadSize", StringValue("1024"));
//2号节点作为生产者
  producerHelper.Install(nodes.Get(2)); 

  Simulator::Stop(Seconds(10.0));

  Simulator::Run();
  Simulator::Destroy();

  return 0;
}

} // namespace ns3

int
main(int argc, char* argv[])
{
  return ns3::main(argc, argv);
}
