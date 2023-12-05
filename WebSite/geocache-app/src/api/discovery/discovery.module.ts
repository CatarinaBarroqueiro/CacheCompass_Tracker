import { Module } from '@nestjs/common';
import { DiscoveryController } from './discovery.controller';
import { DiscoveryService } from './discovery.service';
import { AppService } from 'src/app.service';
import { Discovery } from './discovery.entity';
import { TypeOrmModule } from '@nestjs/typeorm';


@Module({
  imports: [TypeOrmModule.forFeature([Discovery])],
  controllers: [DiscoveryController],
  providers: [DiscoveryService, AppService],
  exports:[DiscoveryService]
})
export class DiscoveryModule {}
