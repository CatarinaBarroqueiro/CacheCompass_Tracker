import { Module } from '@nestjs/common';
import { BoxController } from './box.controller';
import { BoxService } from './box.service';
import { TypeOrmModule } from '@nestjs/typeorm';
import { Box } from './box.entity';
import { AppService } from 'src/app.service';
@Module({
  imports: [TypeOrmModule.forFeature([Box])],
  providers: [BoxService, AppService],
  controllers: [BoxController],
  exports: [BoxService]
})
export class BoxModule {}
