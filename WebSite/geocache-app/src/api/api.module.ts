import { Module } from '@nestjs/common';
import { BoxModule } from './box/box.module';
import { DiscoveryModule } from './discovery/discovery.module';

@Module({
  imports: [BoxModule, DiscoveryModule]
})
export class ApiModule {}
