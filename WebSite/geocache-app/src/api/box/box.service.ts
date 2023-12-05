import { Injectable } from '@nestjs/common';
import { Box } from './box.entity';
import { InjectRepository } from '@nestjs/typeorm';
import { Repository } from 'typeorm';

@Injectable()
export class BoxService {
    constructor(
        @InjectRepository(Box)
        private boxRepository: Repository<Box>
    ) {

    }

    async findAll() {
        return await this.boxRepository.find(
            { loadRelationIds: true}
        );
    }

    async save( box: { idBox: string, local: string}) {
        return await this.boxRepository.save(box);
    }

    async remove(idBox: string) {
        return await this.boxRepository.delete(idBox);
    }
}
