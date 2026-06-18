package com.unipi.PlayerHive.DTO.reviews;

import lombok.AllArgsConstructor;
import lombok.Getter;
import org.bson.types.ObjectId;
import org.springframework.data.annotation.Id;
import org.springframework.data.mongodb.core.mapping.Field;

@AllArgsConstructor
@Getter
public class ReviewScoreDTO {

    @Id
    private String id;

    @Field("game_id")
    private ObjectId gameId;

    private Float score;
}
