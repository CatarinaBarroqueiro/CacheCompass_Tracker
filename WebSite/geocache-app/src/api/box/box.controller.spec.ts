import { Test, TestingModule } from '@nestjs/testing';
import { BoxController } from './box.controller';

describe('BoxController', () => {
  let controller: BoxController;

  beforeEach(async () => {
    const module: TestingModule = await Test.createTestingModule({
      controllers: [BoxController],
    }).compile();

    controller = module.get<BoxController>(BoxController);
  });

  it('should be defined', () => {
    expect(controller).toBeDefined();
  });
});
